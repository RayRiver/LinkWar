#ifndef UserData_h__
#define UserData_h__

class UserData
{
public:
	static UserData *getInstance();
	~UserData();

	inline void setUid(int uid) { m_uid = uid; }
	inline int getUid() { return m_uid; }

private:
	UserData();
	static UserData *s_instance;

	int m_uid;
};

#endif // UserData_h__
