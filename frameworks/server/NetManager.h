#ifndef NetManager_h__
#define NetManager_h__

#include <vector>
#include <thread>
#include <mutex>
#include <functional>

class NetSocket;
class VarList;
class NetManager
{
public:
	static NetManager *instance();
	virtual ~NetManager();

	void start();
	void stop();
	void run();
	inline bool isStarted() { return m_thread ? true : false; }

	void add(NetSocket *sock);
	void remove(NetSocket *sock);
	bool find(NetSocket *sock, size_t &index);

	virtual void update(float dt);

	inline bool hasReadCallback() { return m_hasReadCallback; }
	inline const std::function<void(NetSocket *, unsigned int, const VarList &)> &getReadCallback() { return m_readCallback; }
	inline void registerReadCallback(const std::function<void(NetSocket *, unsigned int, const VarList &)> &fn) { m_hasReadCallback = true; m_readCallback = fn; }

	inline bool hasConnectCallback() { return m_hasConnectCallback; }
	inline const std::function<void(NetSocket *)> &getConnectCallback() { return m_connectCallback; }
	inline void registerConnectCallback(const std::function<void(NetSocket *)> &fn) { m_hasConnectCallback = true; m_connectCallback = fn; }

	inline bool hasDisconnectCallback() { return m_hasDisconnectCallback; }
	inline const std::function<void(NetSocket *)> &getDisconnectCallback() { return m_disconnectCallback; }
	inline void registerDisconnectCallback(const std::function<void(NetSocket *)> &fn) { m_hasDisconnectCallback = true; m_disconnectCallback = fn; }

private:
	NetManager();
	static void thread_task(NetManager *engine);
	static void accept_thread_task(NetManager *engine);

private:
	std::thread *m_thread;
	std::thread *m_acceptThread;
	bool m_bRunning;
	std::vector<NetSocket *> m_socks;
	std::mutex m_socks_mutex;

	std::vector<int> m_acceptConnections;
	std::mutex m_accept_connections_mutex;

	bool m_hasReadCallback;
	std::function<void(NetSocket *, unsigned int, const VarList &)> m_readCallback;
	bool m_hasConnectCallback;
	std::function<void(NetSocket *)> m_connectCallback;
	bool m_hasDisconnectCallback;
	std::function<void(NetSocket *)> m_disconnectCallback;

};

#endif // NetManager_h__
