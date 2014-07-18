#ifndef GameObject_h__
#define GameObject_h__

#include <map>

#include "MapStructs.h"
#include "GameObjectMacros.h"
#include "BlackBoard.h"
#include "PathFinder.h"

class BTNode;

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

	// �Ƿ��ǵ���;
	inline bool isEnemy() const { return m_isEnemy; }

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

	// ��Ҫ�ƶ�����λ����Ϣ;
	inline bool desireMove() const { return m_desireMove; }

	inline const MapPoint &getDesiredPosition() const { return m_desiredPosition; }
	inline const Fixed &getDesiredPositionX() const { return m_desiredPosition.x; }
	inline const Fixed &getDesiredPositionY() const { return m_desiredPosition.y; }

	inline void setDesiredPosition(const MapPoint &point) { m_desiredPosition = point; }
	inline void setDesiredPosition(const Fixed &x, const Fixed &y) { m_desiredPosition.x = x; m_desiredPosition.y = y; }
	inline void setDesiredPositionX(const Fixed &x) { m_desiredPosition.x = x; }
	inline void setDesiredPositionY(const Fixed &y) { m_desiredPosition.x = y; }

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

private:

	struct PropertyValue
	{
		Fixed f;
		std::string s;
	};
	typedef std::map<std::string, PropertyValue> PropertyMap;
	typedef std::pair<std::string, PropertyValue> PropertyMapPair;

	PropertyMap m_propertyMap;				// �������Ա�;
	BlackBoard m_blackboard;				// �������ݺڰ壨��Ϊ�����������������;

	bool m_shouldClean;						// �Ƿ���Ҫ����;

	int m_id;								// ����ID;

	int m_group;							// ������ID;
	bool m_isEnemy;

	GameObjectState m_state;				// ����״̬;

	MapPoint m_position;					// ����λ��;

	bool m_desireMove;						// �����Ƿ���Ҫ�ƶ�;
	MapPoint m_desiredPosition;				// ������Ҫ�ƶ�����λ��;

	Fixed m_moveSpeed;						// �����ƶ��ٶ�;

	BTNode *m_behavior;						// ������Ϊ�����ڵ�;
	int m_behaviorIntervalFrames;			// ������Ϊ���µ�ǰ֡��֡��;
	int m_currentBehaviorIntervalFrames;	// ������Ϊ����Ƶ�ʣ�֡��;

	MapRect m_hitBox;						// �����ܻ���;
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
};

#endif // GameObject_h__
