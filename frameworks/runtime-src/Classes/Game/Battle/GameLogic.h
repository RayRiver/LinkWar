#ifndef GameLogic_h__
#define GameLogic_h__

#include <map>

#include "MapStructs.h"

class LogicFrame;
class GameObject;
class GameLogic
{
public:
	static GameLogic *getInstance();
	~GameLogic();

	void handleDragMap(const cocos2d::Vec2 &vec);
	void handleTouch(const cocos2d::Vec2 &pos);
	void handleLogicFrame(LogicFrame *frame);
	void moveObject(GameObject *object);

private:
	GameLogic();
	static GameLogic *s_instance;

	int m_objectIdIndex;

	
};

#endif // GameLogic_h__
