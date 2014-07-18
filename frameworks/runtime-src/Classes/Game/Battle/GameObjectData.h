#ifndef GameObjectData_h__
#define GameObjectData_h__

#include "cocos2d.h"
#include "CCLuaValue.h"

#include <string>
#include <map>

#include "MapStructs.h"
#include "PathFinder.h"

#include "GameObjectMacros.h"

#include "BehaviorTree/BehaviorTree.h"

class GameObjectData
{
	friend class GameObject;


public:
	GameObjectData();
	~GameObjectData();

	bool init(const char *config, int id, int group);

	// properties;
	void removeProperty(const char *name);
	void setProperty(const char *name, const Fixed &val);
	bool getProperty(const char *name, Fixed &val);
	void setProperty(const char *name, const char *val);
	bool getProperty(const char *name, std::string &val);


private:
	struct PropertyValue
	{
		Fixed f;
		std::string s;
	};
	typedef std::map<std::string, PropertyValue> PropertyMap;
	typedef std::pair<std::string, PropertyValue> PropertyMapPair;
	PropertyMap m_propertyMap;

	bool m_shouldClean;

	int m_id;

	int m_group;
	bool m_isEnemy;

	GameObjectState m_state;

	MapPoint m_position;

	bool m_desireMove;
	MapPoint m_desiredPosition;

	Fixed m_moveSpeed;

	BTNode *m_behavior;
	int m_behaviorIntervalFrames;
	int m_currentBehaviorIntervalFrames;

	MapRect m_hitBox;
	MapRect m_attackArea;


	MapPoint m_moveTarget;

	GameObject *m_attackTarget;
	std::map<int, GameObject *> m_lockSet; // 锁定该实体的实体列表;

	PathFinder m_pathFinder;

	int m_attackInterval;
	int m_currentAttackInterval;

	Fixed m_hp, m_maxhp;
	Fixed m_atk;
};

#endif // GameObjectData_h__
