#ifndef UserData_h__
#define UserData_h__

#include <deque>
#include <mutex>

struct LogicFrameAction
{
	enum class Type
	{
		CreateEntity = 1,
	};

	int uid;
	Type type;
	float x, y;

};

class NetSocket;
class UserData
{
public:
	UserData(NetSocket *conn, int uid);
	~UserData();

	inline NetSocket *getConnection() { return m_conn; }
	inline int getUid() { return m_uid; }

	void addAction(const LogicFrameAction &action);
	bool getAction(LogicFrameAction &action); 

private:
	NetSocket *m_conn;
	int m_uid;

	std::mutex m_actions_mutex;
	std::deque<LogicFrameAction> m_actions;

};

#endif // UserData_h__
