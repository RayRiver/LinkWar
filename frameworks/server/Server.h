#ifndef Server_h__
#define Server_h__

#include <windows.h>

#include <vector>
#include <map>

class UserData;
class VarList;
class NetSocket;
class Server
{
public:
	Server();
	~Server();

	bool init(double interval);
	void run();

	void onConnected(NetSocket *conn);
	void onDisconnect(NetSocket *conn);
	void onReceive(NetSocket *conn, unsigned int id, const VarList &args);

private:
	typedef std::vector<UserData *> UserList;
	typedef std::map<NetSocket *, int> UserMap;
	typedef std::pair<NetSocket *, int> UserMapPair;
	UserList m_userList;
	UserMap m_userMap;

	std::vector<UserData *> m_readyUsers;
	bool m_bStartSyncLogicFrame;
	int m_nEntityId;

// internal bellow -----------------------------------------------
private:
	void calculateDeltaTime();
	void mainLoop();
	void update(float dt);

private:
	LARGE_INTEGER m_interval;
	float m_deltaTime;
	struct timeval *m_lastUpdate;
};

#endif // Server_h__
