#include "Server.h"

#include <windows.h>

#include "Log.h"
#include "ThreadManager.h"
#include "NetManager.h"
#include "VarList.h"
#include "NetSocket.h"

#include "MessageDef.h"
#include "UserData.h"

static int gettimeofday(struct timeval * val, struct timezone *)
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency( &liFreq );
		QueryPerformanceCounter( &liTime );
		val->tv_sec     = (long)( liTime.QuadPart / liFreq.QuadPart );
		val->tv_usec    = (long)( liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0 );
	}
	return 0;
}

Server::Server()
	: m_lastUpdate(nullptr)
	, m_bStartSyncLogicFrame(false)
	, m_nEntityId(0)
{

}

Server::~Server()
{
	for (auto userdata : m_userList)
	{
		if (userdata)
		{
			delete userdata;
		}
	}
	m_userList.clear();
	m_userMap.clear();

	if (m_lastUpdate)
	{
		delete m_lastUpdate;
		m_lastUpdate = nullptr;
	}
}

bool Server::init(double interval)
{
	m_lastUpdate = new struct timeval;

	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	m_interval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);

	ThreadManager::instance();
	NetManager::instance()->start();

	NetManager::instance()->registerConnectCallback([this](NetSocket *conn) {
		this->onConnected(conn);
	});
	NetManager::instance()->registerDisconnectCallback([this](NetSocket *conn) {
		this->onDisconnect(conn);
	});
	NetManager::instance()->registerReadCallback([this](NetSocket *conn, unsigned int id, const VarList &args) {
		this->onReceive(conn, id, args);
	});

	Log("Server init complete");

	return true;
}

void Server::run()
{
	this->mainLoop();
}

void Server::calculateDeltaTime()
{
	struct timeval now;

	if (gettimeofday(&now, nullptr) != 0)
	{
		Log("error in gettimeofday");
		m_deltaTime = 0;
		return;
	}

	m_deltaTime = (now.tv_sec - m_lastUpdate->tv_sec) + (now.tv_usec - m_lastUpdate->tv_usec) / 1000000.0f;
	if (m_deltaTime < 0.0) m_deltaTime = 0.0;

	*m_lastUpdate = now;
}

void Server::mainLoop()
{
	// Main message loop:
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nLast;
	LARGE_INTEGER nNow;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLast);

	while(true)
	{
		QueryPerformanceCounter(&nNow);
		if (nNow.QuadPart - nLast.QuadPart > m_interval.QuadPart)
		{
			nLast.QuadPart = nNow.QuadPart;

			calculateDeltaTime();
			update(m_deltaTime);
		}
		else
		{
			Sleep(0);
		}
	}
}

void Server::update(float dt)
{
	ThreadManager::instance()->update(dt);
	NetManager::instance()->update(dt);

	if (m_bStartSyncLogicFrame)
	{
		// find all actions
		std::vector<LogicFrameAction> vActions;
		for (auto ready_user : m_readyUsers)
		{
			LogicFrameAction action;
			if (ready_user && ready_user->getAction(action))
			{
				vActions.push_back(action);
			}
		}

		VarList args;
		args.add((int)vActions.size());
		for (const auto &action : vActions)
		{
			int pos = -1;
			for (int i=0; i<(int)m_readyUsers.size(); ++i)
			{
				if (action.uid == m_readyUsers[i]->getUid())
				{
					pos = i;
					break;
				}
			}
			args.add((int)pos);
			args.add((int)action.type);
			args.add((float)action.x);
			args.add((float)action.y);
			args.add((int)action.id);
		}

		for (auto ready_user : m_readyUsers)
		{
			size_t index = 0;
			auto conn = (ready_user->getConnection());
			if (NetManager::instance()->find(conn, index))
			{
				ready_user->getConnection()->writePacket(NET_SC_LOGIC_FRAME, args);	
			}
		}
	}
}

void Server::onConnected( NetSocket *conn )
{
	int uid = m_userList.size();
	m_userList.push_back(new UserData(conn, uid));
	m_userMap.insert(UserMapPair(conn, uid));
	Log("user %d enter, connection: 0x%08x", uid, conn);
}

void Server::onDisconnect( NetSocket *conn )
{
	auto it = m_userMap.find(conn);
	if (it == m_userMap.end())
	{
		Log("connection 0x%08x lost, cannot find uid", conn);
		return;
	}
	int uid = it->second;

	delete m_userList[uid];
	m_userList[uid] = nullptr;

	m_userMap.erase(it);

	Log("user %d leave, connection: 0x%08x", uid, conn);
}

void Server::onReceive( NetSocket *conn, unsigned int id, const VarList &args )
{
	auto it = m_userMap.find(conn);
	if (it == m_userMap.end())
	{
		Log("connection 0x%08x send message, cannot find uid", conn);
		return;
	}
	int uid = it->second;
	UserData *user = m_userList[uid];

	switch (id)
	{
	case NET_CS_READY:
		{
			Log("recv ready msg from uid=%d", uid); 

			if (m_readyUsers.size() < 2)
			{
				m_readyUsers.push_back(user);
				if (m_readyUsers.size() == 2)
				{
					for (int i=0; i<(int)m_readyUsers.size(); ++i)
					{
						auto ready_user = m_readyUsers[i];
						VarList send_args;
						send_args.add(i);
						ready_user->getConnection()->writePacket(NET_SC_START, send_args);	
						Log("send start msg to uid=%d", ready_user->getUid()); 
					}

					m_bStartSyncLogicFrame = true;
				}
			}
		}
		break;
	case NET_CS_CREATE_ENTITY:
		{
			Log("recv create entity msg from uid=%d", uid); 
			
			float x = args.get(0).toFloat();
			float y = args.get(1).toFloat();

			LogicFrameAction action;
			action.uid = uid;
			action.type = LogicFrameAction::Type::CreateEntity;
			action.x = x;
			action.y = y;
			action.id = ++m_nEntityId;

			user->addAction(action);
		}
		break;
	default:
		Log("unknown message id: %u", id);
	}

}

