#ifndef GameLogic_h__
#define GameLogic_h__

#include <map>

class LogicFrame;
class GameObject;
class GameLogic
{
public:
	static GameLogic *getInstance();
	~GameLogic();

	void handleLogicFrame(LogicFrame *frame);
	void moveObject(GameObject *object);

private:
	GameLogic();
	static GameLogic *s_instance;

	std::map<int, GameObject *> m_shouldCleanObjects;
};

#endif // GameLogic_h__
