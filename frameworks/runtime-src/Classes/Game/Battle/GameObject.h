#ifndef GameObject_h__
#define GameObject_h__

#include <map>

#include "MapStructs.h"
#include "GameObjectMacros.h"
#include "BlackBoard.h"
#include "PathFinder.h"

class BTNode;
class ObjectComponent;

class GameObject
{
public:
	GameObject();
	~GameObject();

	bool init(const char *config, int id, int group);
	bool initData(const char *config);

	// 逻辑帧处理;
	void onLogic();

	// 对象行为;
	void idle();
	void move(const MapPoint &pos);
	void moveToAttack();
	void attack();
	void hit(GameObject *entity);
	void die();

	// 对象组件操作方法;
	void addComponent(const char *name, ObjectComponent *com);
	void removeComponent(const char *name);
	void removeAllComponents();
	ObjectComponent *getComponent(const char *name);

	template<class T>
	T *addComponent()
	{
		const char *classname = typeid(T).name();
		ObjectComponent *com = new T;
		if (com)
		{
			com->addComponent(classname, com);
		}
		return dynamic_cast<T *>(com);
	}
	template<class T>
	void removeComponent()
	{
		const char *classname = typeid(T).name();
		removeComponent(classname);	
	}
	template<class T>
	T *getComponent()
	{
		const char *classname = typeid(T).name();
		auto com = getComponent(name);
		if (com)
		{
			return dynamic_cast<T *>(com);
		}
		return nullptr;
	}

	// 对象属性读写方法;
	void removeProperty(const char *name);
	void setProperty(const char *name, const Fixed &val);
	bool getProperty(const char *name, Fixed &val);
	void setProperty(const char *name, const char *val);
	bool getProperty(const char *name, std::string &val);

	// 对象是否需要清理;
	inline bool shouldClean() const { return m_shouldClean; }

	// 对象ID和组;
	inline int id() const { return m_id; }
	inline int group() const { return m_group; }

	// 对象属性;
	inline const Fixed &hp() const { return m_hp; }
	inline const Fixed &maxhp() const { return m_maxhp; }
	inline const Fixed &atk() const { return m_atk; }

	// 对象状态;
	inline GameObjectState getState() { return m_state; }
	inline void setState(GameObjectState state) { m_state = state; }

	// 位置信息;
	inline const MapPoint &getPosition() const { return m_position; }
	inline const Fixed &getPositionX() const { return m_position.x; }
	inline const Fixed &getPositionY() const { return m_position.y; }

	void setPosition(const MapPoint &point);
	void setPosition(const Fixed &x, const Fixed &y);
	void setPositionX(const Fixed &x);
	void setPositionY(const Fixed &y);

	// 对象朝向;
	inline GameObjectDirection getDirection() { return m_direction; }
	inline void setDirection(GameObjectDirection direction) { m_direction = direction; }

	// 移动速度;
	inline void setMoveSpeed(const Fixed &speed) { m_moveSpeed = speed; }
	inline const Fixed &getMoveSpeed() { return m_moveSpeed; }

	// 移动到的目标位置;
	inline void setMoveTarget(const MapPoint &point) { m_moveTarget = point; }
	inline MapPoint getMoveTarget() const { return m_moveTarget ; }

	// 攻击范围;
	inline void setAttackArea(const MapRect &area) { m_attackArea = area; }
	inline const MapRect &getAttackArea() { return m_attackArea; }

	// 攻击目标;
	void setAttackTarget(GameObject *target);
	inline GameObject *getAttackTarget() const { return m_attackTarget; }

	// 锁定该实体的实体集合;
	inline std::map<int, GameObject *> &lockSet() { return m_lockSet; }

	// 对象寻路;
	inline PathFinder &getPathFinder() { return m_pathFinder; }

	// 对象受击盒;
	inline MapRect &getHitBox() { return m_hitBox; }

	// 是否显示受击盒;
	inline bool isShowHitBox() { return m_showHitBox; }

	// 动画信息;
	inline const char *getAnimationFile() { return m_animationFile.empty() ? nullptr : m_animationFile.c_str(); }
	inline const char *getAnimationName() { return m_animationName.empty() ? nullptr : m_animationName.c_str(); }
	inline const char *getAnimationDefaultAction() { return m_animationDefaultAction.empty() ? nullptr : m_animationDefaultAction.c_str(); }
	inline const Fixed &getAnimationScale() { return m_animationScale; }

	// 逻辑帧内移动目标位置;
	inline bool hasFrameMoveTarget() { return m_hasFrameMoveTarget; }
	inline void setHasFrameMoveTarget(bool _hasFrameMoveTarget) { m_hasFrameMoveTarget = _hasFrameMoveTarget; }
	inline const MapPoint &getFrameMoveTarget() { return m_frameMoveTarget; }
	inline void setFrameMoveTarget(const MapPoint &point) { m_frameMoveTarget = point; }

private:
	typedef std::map<std::string, ObjectComponent *> ComponentMap;
	typedef std::pair<std::string, ObjectComponent *> ComponentMapPair;

	struct PropertyValue
	{
		Fixed f;
		std::string s;
	};
	typedef std::map<std::string, PropertyValue> PropertyMap;
	typedef std::pair<std::string, PropertyValue> PropertyMapPair;

private:
	ComponentMap m_componentMap;			// 对象组件表;
	PropertyMap m_propertyMap;				// 对象属性表;
	BlackBoard m_blackboard;				// 对象数据黑板（行为树的输入输出参数）;

	bool m_shouldClean;						// 是否需要清理;

	int m_id;								// 对象ID;

	int m_group;							// 对象组ID;

	GameObjectState m_state;				// 对象状态;

	GameObjectDirection m_direction;		// 对象朝向;

	MapPoint m_position;					// 对象位置;


	Fixed m_moveSpeed;						// 对象移动速度;

	BTNode *m_behavior;						// 对象行为树根节点;
	int m_behaviorIntervalFrames;			// 对象行为更新当前帧（帧）;
	int m_currentBehaviorIntervalFrames;	// 对象行为更新频率（帧）;

	MapRect m_hitBox;						// 对象受击盒;
	bool m_showHitBox;						// 是否显示受击盒;
	MapRect m_attackArea;					// 对象攻击区域;


	MapPoint m_moveTarget;					// 对象目标移动位置;

	GameObject *m_attackTarget;
	std::map<int, GameObject *> m_lockSet;	// 锁定该实体的实体列表;

	PathFinder m_pathFinder;				// 对象寻路;

	int m_attackInterval;					// 对象攻击间隔（帧）;
	int m_currentAttackInterval;			// 对象攻击当前帧;

	Fixed m_hp;								// 对象当前血量;
	Fixed m_maxhp;							// 对象最大血量;
	Fixed m_atk;							// 对象攻击力;

	std::string m_animationFile;			// 动画文件名;
	std::string m_animationName;			// 动画名;
	std::string m_animationDefaultAction;	// 默认动作名;
	Fixed m_animationScale;					// 动画拉伸倍数;

	bool m_hasFrameMoveTarget;				// 在逻辑帧内是否需要移动;
	MapPoint m_frameMoveTarget;				// 在逻辑帧内移动的目标位置;
};

#endif // GameObject_h__
