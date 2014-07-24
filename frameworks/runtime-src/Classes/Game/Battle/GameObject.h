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

	// �߼�֡����;
	void onLogic();

	// ������Ϊ;
	void idle();
	void move(const MapPoint &pos);
	void moveToAttack();
	void attack();
	void hit(GameObject *entity);
	void die();

	// ���������������;
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

	// �������Զ�д����;
	void removeProperty(const char *name);
	void setProperty(const char *name, const Fixed &val);
	bool getProperty(const char *name, Fixed &val);
	void setProperty(const char *name, const char *val);
	bool getProperty(const char *name, std::string &val);

	// �����Ƿ���Ҫ����;
	inline bool shouldClean() const { return m_shouldClean; }

	// ����ID����;
	inline int id() const { return m_id; }
	inline int group() const { return m_group; }

	// ��������;
	inline const Fixed &hp() const { return m_hp; }
	inline const Fixed &maxhp() const { return m_maxhp; }
	inline const Fixed &atk() const { return m_atk; }

	// ����״̬;
	inline GameObjectState getState() { return m_state; }
	inline void setState(GameObjectState state) { m_state = state; }

	// λ����Ϣ;
	inline const MapPoint &getPosition() const { return m_position; }
	inline const Fixed &getPositionX() const { return m_position.x; }
	inline const Fixed &getPositionY() const { return m_position.y; }

	void setPosition(const MapPoint &point);
	void setPosition(const Fixed &x, const Fixed &y);
	void setPositionX(const Fixed &x);
	void setPositionY(const Fixed &y);

	// ������;
	inline GameObjectDirection getDirection() { return m_direction; }
	inline void setDirection(GameObjectDirection direction) { m_direction = direction; }

	// �ƶ��ٶ�;
	inline void setMoveSpeed(const Fixed &speed) { m_moveSpeed = speed; }
	inline const Fixed &getMoveSpeed() { return m_moveSpeed; }

	// �ƶ�����Ŀ��λ��;
	inline void setMoveTarget(const MapPoint &point) { m_moveTarget = point; }
	inline MapPoint getMoveTarget() const { return m_moveTarget ; }

	// ������Χ;
	inline void setAttackArea(const MapRect &area) { m_attackArea = area; }
	inline const MapRect &getAttackArea() { return m_attackArea; }

	// ����Ŀ��;
	void setAttackTarget(GameObject *target);
	inline GameObject *getAttackTarget() const { return m_attackTarget; }

	// ������ʵ���ʵ�弯��;
	inline std::map<int, GameObject *> &lockSet() { return m_lockSet; }

	// ����Ѱ·;
	inline PathFinder &getPathFinder() { return m_pathFinder; }

	// �����ܻ���;
	inline MapRect &getHitBox() { return m_hitBox; }

	// �Ƿ���ʾ�ܻ���;
	inline bool isShowHitBox() { return m_showHitBox; }

	// ������Ϣ;
	inline const char *getAnimationFile() { return m_animationFile.empty() ? nullptr : m_animationFile.c_str(); }
	inline const char *getAnimationName() { return m_animationName.empty() ? nullptr : m_animationName.c_str(); }
	inline const char *getAnimationDefaultAction() { return m_animationDefaultAction.empty() ? nullptr : m_animationDefaultAction.c_str(); }
	inline const Fixed &getAnimationScale() { return m_animationScale; }

	// �߼�֡���ƶ�Ŀ��λ��;
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
	ComponentMap m_componentMap;			// ���������;
	PropertyMap m_propertyMap;				// �������Ա�;
	BlackBoard m_blackboard;				// �������ݺڰ壨��Ϊ�����������������;

	bool m_shouldClean;						// �Ƿ���Ҫ����;

	int m_id;								// ����ID;

	int m_group;							// ������ID;

	GameObjectState m_state;				// ����״̬;

	GameObjectDirection m_direction;		// ������;

	MapPoint m_position;					// ����λ��;


	Fixed m_moveSpeed;						// �����ƶ��ٶ�;

	BTNode *m_behavior;						// ������Ϊ�����ڵ�;
	int m_behaviorIntervalFrames;			// ������Ϊ���µ�ǰ֡��֡��;
	int m_currentBehaviorIntervalFrames;	// ������Ϊ����Ƶ�ʣ�֡��;

	MapRect m_hitBox;						// �����ܻ���;
	bool m_showHitBox;						// �Ƿ���ʾ�ܻ���;
	MapRect m_attackArea;					// ���󹥻�����;


	MapPoint m_moveTarget;					// ����Ŀ���ƶ�λ��;

	GameObject *m_attackTarget;
	std::map<int, GameObject *> m_lockSet;	// ������ʵ���ʵ���б�;

	PathFinder m_pathFinder;				// ����Ѱ·;

	int m_attackInterval;					// ���󹥻������֡��;
	int m_currentAttackInterval;			// ���󹥻���ǰ֡;

	Fixed m_hp;								// ����ǰѪ��;
	Fixed m_maxhp;							// �������Ѫ��;
	Fixed m_atk;							// ���󹥻���;

	std::string m_animationFile;			// �����ļ���;
	std::string m_animationName;			// ������;
	std::string m_animationDefaultAction;	// Ĭ�϶�����;
	Fixed m_animationScale;					// �������챶��;

	bool m_hasFrameMoveTarget;				// ���߼�֡���Ƿ���Ҫ�ƶ�;
	MapPoint m_frameMoveTarget;				// ���߼�֡���ƶ���Ŀ��λ��;
};

#endif // GameObject_h__
