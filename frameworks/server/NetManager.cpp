#include "NetManager.h"

#include <assert.h>

#include "Sys.h"
#include "NetDef.h"
#include "NetSocket.h"
#include "Log.h"

using namespace std;

NetManager * NetManager::instance()
{
	static NetManager obj;
	return &obj;
}

void NetManager::thread_task(NetManager *engine)
{
	if (engine)
	{
		engine->run();
	}
}

void NetManager::accept_thread_task(NetManager *engine)
{
	//创建套接字
	auto sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
	if (INVALID_SOCKET == sock)
	{
		Log("socket error");
		return;
	}

	//服务器套接字地址 
	SOCKADDR_IN	addrServ;
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(30001);
	addrServ.sin_addr.s_addr = INADDR_ANY;		

	//绑定套接字
	int retVal = ::bind(sock, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{	
		Log("bind error");
		closesocket(sock);	//关闭套接字
		return;
	}

	//开始监听 
	retVal = ::listen(sock, 1024);
	if (SOCKET_ERROR == retVal)
	{
		Log("listener error");
		closesocket(sock);	//关闭套接字
		return;
	}

	//接受客户端请求
	while (engine->m_bRunning)
	{
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		SOCKET conn = accept(sock, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == conn)
		{
			Log("accept error");
			closesocket(sock);	//关闭套接字
			return;
		}

		// none block mode
		u_long arg = 1;
		::ioctlsocket(conn, FIONBIO, &arg);

		//auto s = createNetSocket();
		//if (s)
		{
			//s->setfd(conn);	
			//engine->add(s);

			lock_guard<mutex> lock(engine->m_accept_connections_mutex);
			engine->m_acceptConnections.push_back(conn);
		}
	}
}

NetManager::NetManager()
	: m_thread(nullptr)
	, m_acceptThread(nullptr)
	, m_bRunning(false)
	, m_hasConnectCallback(false)
	, m_hasDisconnectCallback(false)
	, m_hasReadCallback(false)
{
#ifdef _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
	}
#endif
}

NetManager::~NetManager()
{
	if (m_thread && m_acceptThread && m_bRunning)
	{
		this->stop();
	}
#ifdef _WIN32
	WSACleanup();
#endif
}

void NetManager::start()
{
	if (m_thread) 
	{
		assert(false);
		return;
	}

	m_thread = new thread(NetManager::thread_task, this);
	m_acceptThread = new thread(NetManager::accept_thread_task, this);
}

void NetManager::stop()
{
	if (m_bRunning)
	{
		{
			lock_guard<mutex> lock(m_socks_mutex);
			for (auto sock : m_socks)
			{
				sock->disconnect();
			}
			m_socks.clear();
		}

		m_bRunning = false;

		m_thread->join();	
		delete m_thread;
		m_thread = nullptr;

		m_acceptThread->join();
		delete m_acceptThread;
		m_acceptThread = nullptr;
	}
}

void NetManager::run()
{
	m_bRunning = true;

	fd_set readable;
	fd_set writable;
	fd_set exception;

	int nfds;
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;

	vector<NetSocket *> socks;
	while (m_bRunning)
	{
		size_t num = m_socks.size();
		if (num == 0)
		{
			Sys::sleep(100);
			continue;
		}

		FD_ZERO(&readable);
		FD_ZERO(&writable);
		FD_ZERO(&exception);

		int fd_max = 0;
		socks.clear();
		if (num > 0)
		{
			lock_guard<mutex> lock(m_socks_mutex);
			for (auto sock : m_socks)
			{
				int fd = sock->fd();
				if (fd == INVALID_SOCKET)
				{
					continue;
				}
				socks.push_back(sock);

				if (fd_max < fd) fd_max = fd;

				if (sock->getWriteStream().size() > 0)
				{
					FD_SET(fd, &writable);
				}
				else
				{
					FD_SET(fd, &readable);
				}
				FD_SET(fd, &exception);
			}
		}

		nfds = select(fd_max+1, &readable, &writable, &exception, &timeout);
		if (nfds > 0)
		{
			for (auto sock : socks)
			{
				if (!sock->isValid()) continue;

				int fd = sock->fd();
				if (FD_ISSET(fd, &exception))
				{
					// TODO
					sock->do_error(0);
				}
				if (sock->getWriteStream().size() > 0)
				{
					if (FD_ISSET(fd, &writable))	
					{
						sock->do_write();
					}
				}
				else
				{
					if (FD_ISSET(fd, &readable))
					{
						sock->do_read();
					}
				}
			}
		}

		Sys::sleep(2);
	}
}

void NetManager::update(float dt)
{
	vector<NetSocket *> deleteList;
	for (auto sock : m_socks)
	{
		if (sock->isValid())
		{
			sock->update();
		}
		else
		{
			deleteList.push_back(sock);
		}
	}

	for (auto sock : deleteList)
	{
		this->remove(sock);
	}

	// new connection
	{
		lock_guard<mutex> lock(m_accept_connections_mutex);
		for (auto conn : m_acceptConnections)
		{
			auto s = createNetSocket();
			s->setfd(conn);
			this->add(s);
			s->onConnected();
		}
		m_acceptConnections.clear();
	}
}

void NetManager::add( NetSocket *sock )
{
	lock_guard<mutex> lock(m_socks_mutex);

	size_t index = 0;
	if (this->find(sock, index))
	{
		assert(false);
		return;
	}

	m_socks.push_back(sock);
}

void NetManager::remove( NetSocket *sock )
{
	lock_guard<mutex> lock(m_socks_mutex);

	size_t index = 0;
	if (!this->find(sock, index))
	{
		assert(false);
		return;
	}

	m_socks.erase(m_socks.begin() + index);
	delete sock;
}

bool NetManager::find( NetSocket *sock, size_t &index )
{
	for (size_t i=0; i<m_socks.size(); ++i)
	{
		if (m_socks[i] == sock)
		{
			index = i;
			return true;
		}
	}
	return false;
}


