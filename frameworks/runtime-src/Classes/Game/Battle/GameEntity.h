#ifndef GameEntity_h__
#define GameEntity_h__

#include <string>
#include <set>
#include <map>

#include "cocos2d.h"
#include "CCLuaValue.h"

#include "BlackBoard.h"
#include "PathFinder.h"

class MapManager;
class GameEntity : public cocos2d::Node
{
public:
	enum class State
	{
		Idle,
		Move,
		MoveToAttack,
		Attack,
		Die,
	};

public:
	static GameEntity *create(const char *filename, MapManager *map, bool isEnemy);
	bool init(const char *filename, MapManager *map, bool isEnemy);

	bool loadFromFile(const char *filename);

	void addProperty(const char *name, const cocos2d::LuaValue &val);
	void setProperty(const char *name, const cocos2d::LuaValue &val);
	void removeProperty(const char *name);
	bool getProperty(const char *name, cocos2d::LuaValue &val);

	virtual void onFrame(float dt);
	virtual void onEvaluate(float dt);

	inline bool isEnemy() { return m_isEnemy; }

	void updateZOrder();
	void moveToPosition(const cocos2d::Vec2 &pos);

	void idle();
	void move(const cocos2d::Vec2 &pos);
	void moveToAttack();
	void attack();
	void hit(GameEntity *entity);
	void die();

	inline void setHitBox(const cocos2d::Rect &box) { m_hitBox.origin.x=box.origin.x; m_hitBox.origin.y=box.origin.y; m_hitBox.size.width=box.size.width; m_hitBox.size.height=box.size.height; }
	inline const cocos2d::Rect &getHitBox() { return m_hitBox; }

	inline void setAttackBox(const cocos2d::Rect &box) { m_attackBox.origin.x=box.origin.x; m_attackBox.origin.y=box.origin.y; m_attackBox.size.width=box.size.width; m_attackBox.size.height=box.size.height; }
	inline const cocos2d::Rect &getAttackBox() { return m_attackBox; }	

	inline void setMoveTarget(const cocos2d::Vec2 &pos) { m_moveTarget.x = pos.x; m_moveTarget.y = pos.y; }
	inline const cocos2d::Vec2 &getMoveTarget() { return m_moveTarget; }

	void setAttackTarget(GameEntity *target);
	inline GameEntity *getAttackTarget() { return m_attackTarget; }

	// 锁定该实体的实体集合
	inline std::set<GameEntity *> &lockSet() { return m_lockSet; }

	inline void setState(State state) { m_state = state; }
	inline State getState() { return m_state; }

	inline void setAttackArea(const cocos2d::Rect &area) { m_attackArea.origin.x=area.origin.x; m_attackArea.origin.y=area.origin.y; m_attackArea.size.width=area.size.width; m_attackArea.size.height=area.size.height; }
	inline const cocos2d::Rect &getAttackArea() { return m_attackArea; }

	inline bool shouldClean() { return m_bShouldClean; }

	void updateHPBar();

	inline PathFinder &getPathFinder() { return m_pathFinder; }

	inline float getMoveSpeed() { return m_moveSpeed; }
	inline void setMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }


	inline bool desireMove() { return m_desireMove; }
	inline void setDesiredPosition(const cocos2d::Vec2 &pos) { m_desiredPostition.x = pos.x; m_desiredPostition.y = pos.y; }
	inline const cocos2d::Vec2 &getDesiredPosition() { return m_desiredPostition; }

private:
	typedef std::map<std::string, cocos2d::LuaValue> PROPERTY_MAP;
	typedef std::pair<std::string, cocos2d::LuaValue> PROPERTY_MAP_PAIR;
	PROPERTY_MAP m_propertyMap;

	BlackBoard m_blackboard;

	bool m_isEnemy;
	cocos2d::Rect m_hitBox;
	cocos2d::Rect m_attackBox;

	State m_state;
	cocos2d::Vec2 m_moveTarget;

	float m_moveSpeed;

	cocos2d::Rect m_attackArea;

	GameEntity *m_attackTarget;
	std::set<GameEntity *> m_lockSet; // 锁定该实体的实体列表

	bool m_hasBehavior;
	float m_behaviorInterval;

	bool m_bShouldClean;

	cocos2d::DrawNode *m_hpBar;

	float m_currentBehaviorInterval;

	PathFinder m_pathFinder;

	bool m_desireMove;
	cocos2d::Vec2 m_desiredPostition;
};

#endif // GameEntity_h__
