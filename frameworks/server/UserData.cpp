#include "UserData.h"

#include "NetSocket.h"

using namespace std;

UserData::UserData(NetSocket *conn, int uid)
	: m_conn(conn)
	, m_uid(uid)
{

}

UserData::~UserData()
{

}

void UserData::addAction( const LogicFrameAction &action )
{
	lock_guard<mutex> lock(m_actions_mutex); 
	m_actions.push_back(action);
}

bool UserData::getAction( LogicFrameAction &action )
{
	lock_guard<mutex> lock(m_actions_mutex); 
	if (m_actions.size() > 0)
	{ 
		action = m_actions.front(); 
		m_actions.pop_front();
		return true;
	}
	else
	{
		return false;
	}
}

