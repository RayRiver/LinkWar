#include "GameObject.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

#include "Helper/Display.h"
#include "BehaviorTree/BehaviorTree.h"

#include "GameObjectView.h"
#include "GameObjectManager.h"
#include "PathFinder.h"
#include "ObjectComponent.h"

USING_NS_CC;
using namespace cocostudio;

GameObject::GameObject()
	: m_shouldClean(false)
	, m_id(-1)
	, m_group(-1)
	, m_state(GameObjectState::Idle)
	, m_desireMove(false)
	, m_direction(GameObjectDirection::Down)
	, m_showHitBox(false)
	, m_behavior(nullptr)
	, m_behaviorIntervalFrames(1)
	, m_currentBehaviorIntervalFrames(0)
	, m_attackTarget(nullptr)
	, m_attackInterval(30)
	, m_currentAttackInterval(m_attackInterval)
{

}

GameObject::~GameObject()
{
	if (m_behavior)
	{
		delete m_behavior;
		m_behavior = nullptr;
	}

	removeAllComponents();
}

bool GameObject::init( const char * config, int id, int group )
{
	m_id = id;
	m_group = group;

	// ��ʼ����������;
	if (!this->initData(config))
	{
		return false;
	}

	// ��ʼ��Ѱ·��Ϣ;
	m_pathFinder.init(MAP);

	// ��ʼ���ڰ�;
	m_blackboard.self = this;

	return true;
}

bool GameObject::initData( const char *config )
{
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(config);
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	auto doc = rapidjson::Document();
	doc.Parse<0>(fileContent.c_str());
	if (doc.HasParseError()) {
		CCLOG("GetParseError %s\n", doc.GetParseError());
		return false;
	}

	const rapidjson::Value &dict = doc;

	// ������������;
	m_maxhp = Fixed(DICTOOL->getFloatValue_json(dict, "maxhp", 1.0f));
	m_atk = Fixed(DICTOOL->getFloatValue_json(dict, "atk", 0.0f));
	m_hp = m_maxhp;
	m_moveSpeed = DICTOOL->getFloatValue_json(dict, "move_speed", 0.0f);

	// ����������Ϊ��;
	const char *behavior_config = DICTOOL->getStringValue_json(dict, "behavior", nullptr);
	if (behavior_config)
	{
		m_behavior = BTReader::getInstance()->readFromFile(behavior_config);
	}

	// ������Ϊ�����¼��֡;
	m_behaviorIntervalFrames = DICTOOL->getFloatValue_json(dict, "behavior_interval_frames", m_behaviorIntervalFrames);

	// ���������ܻ���;
	auto hitboxExist = DICTOOL->checkObjectExist_json(dict, "hitbox");
	if (hitboxExist)
	{
		const auto &hitbox = DICTOOL->getSubDictionary_json(dict, "hitbox");
		m_hitBox.x = DICTOOL->getFloatValue_json(hitbox, "x", 0.0f);
		m_hitBox.y = DICTOOL->getFloatValue_json(hitbox, "y", 0.0f);
		m_hitBox.w = DICTOOL->getFloatValue_json(hitbox, "w", 0.0f);
		m_hitBox.h = DICTOOL->getFloatValue_json(hitbox, "h", 0.0f);
	}

	// �����Ƿ���ʾ�ܻ���;
	m_showHitBox = DICTOOL->getBooleanValue_json(dict, "show_hitbox", false);

	// �������󹥻�����;
	auto attackAreaExist = DICTOOL->checkObjectExist_json(dict, "attack_area");
	if (attackAreaExist)
	{
		const auto &attackArea = DICTOOL->getSubDictionary_json(dict, "attack_area");
		m_attackArea.x = DICTOOL->getFloatValue_json(attackArea, "x", 0.0f);
		m_attackArea.y = DICTOOL->getFloatValue_json(attackArea, "y", 0.0f);
		m_attackArea.w = DICTOOL->getFloatValue_json(attackArea, "w", 0.0f);
		m_attackArea.h = DICTOOL->getFloatValue_json(attackArea, "h", 0.0f);
	}

	// �������󶯻�;
	const char *animation_file = DICTOOL->getStringValue_json(dict, "animation_file", nullptr);
	const char *animation_name = DICTOOL->getStringValue_json(dict, "animation_name", nullptr);
	if (animation_file && animation_name)
	{
		m_animationFile = animation_file;
		m_animationName = animation_name;

		const char *animation_default_action = DICTOOL->getStringValue_json(dict, "animation_default_action", nullptr);
		if (animation_default_action)
		{
			m_animationDefaultAction = animation_default_action;
		}

		const auto &animation_scale = Fixed(DICTOOL->getFloatValue_json(dict, "animation_scale", 1.0f));
		m_animationScale = animation_scale;
	}

	return true;
}

void GameObject::addComponent( const char *name, ObjectComponent *com )
{
	auto it = m_componentMap.find(name);
	if (it == m_componentMap.end())
	{
		m_componentMap.insert(ComponentMapPair(name, com));
	}
	else
	{
		log("game object add component [%s]: already exist", name);
	}
}

void GameObject::removeComponent( const char *name )
{
	auto it = m_componentMap.find(name);
	if (it == m_componentMap.end())
	{
		log("game object remove component [%s]: no such component", name);
	}
	else
	{
		auto com = it->second;
		delete com;

		m_componentMap.erase(it);
	}
}

void GameObject::removeAllComponents()
{
	for (auto it : m_componentMap)
	{
		auto com = it.second;
		if (com)
		{
			delete com;
		}
	}
	m_componentMap.clear();
}

ObjectComponent * GameObject::getComponent( const char *name )
{
	auto it = m_componentMap.find(name);
	if (it != m_componentMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void GameObject::removeProperty( const char *name )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		log("game object remove property [%s]: no such property", name);
	}
	else
	{
		m_propertyMap.erase(it);
	}
}

void GameObject::setProperty( const char *name, const Fixed &val )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		PropertyValue propertyValue;		
		propertyValue.f = val;
		m_propertyMap.insert(PropertyMapPair(name, propertyValue));
	}
	else
	{
		it->second.f = val;
	}
}

bool GameObject::getProperty( const char *name, Fixed &val )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		return false;
	}
	else
	{
		val = it->second.f;
		return true;
	}
}

void GameObject::setProperty( const char *name, const char *val )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		PropertyValue propertyValue;		
		propertyValue.s = val;
		m_propertyMap.insert(PropertyMapPair(name, propertyValue));
	}
	else
	{
		it->second.s = val;
	}
}

bool GameObject::getProperty( const char *name, std::string &val )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		return false;
	}
	else
	{
		val = it->second.s;
		return true;
	}
}

void GameObject::onLogic()
{
	// ����AI;
	if (m_behavior)
	{
		++m_currentBehaviorIntervalFrames;
		if (m_currentBehaviorIntervalFrames > m_behaviorIntervalFrames)
		{
			const BTInputParam &input = BTInputParam((void *)&m_blackboard);
			BTOutputParam &output = BTOutputParam((void *)&m_blackboard);
			if (m_behavior->evaluate(input))
			{
				m_behavior->update(input, output);
			}
			m_currentBehaviorIntervalFrames = 0;
		}
	}

	// ��ʼ�� �Ƿ���Ҫ�ƶ�;
	m_desireMove = false;

	// ״̬����;
	auto state = m_state;
	if (state == GameObjectState::Move)
	{
		m_desiredPosition = m_moveTarget;
		m_desireMove = true;
	}
	else if (state == GameObjectState::MoveToAttack)
	{
		auto attackTarget = m_attackTarget;
		if (attackTarget)
		{
			m_desiredPosition = attackTarget->getPosition();
			m_desireMove = true;
		}
	}
	else if (state == GameObjectState::Attack)
	{
		--m_currentAttackInterval;

		if (m_currentAttackInterval == 0)
		{
			auto target = this->getAttackTarget();
			if (target)
			{
				GAME_OBJECT_VIEW(m_id)->onAttack();
				target->hit(this);
			}
			m_currentAttackInterval = m_attackInterval;
		}
	}
}

void GameObject::idle()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Idle);

	// ������ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onIdle();
}

void GameObject::move( const MapPoint &pos )
{
	m_pathFinder.clearPath();
	this->setMoveTarget(pos);
	this->setState(GameObjectState::Move);

	// ������ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onMove();
}

void GameObject::moveToAttack()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::MoveToAttack);

	// ������ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onMove();
}

void GameObject::attack()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Attack);

	m_currentAttackInterval = m_attackInterval;

	// ������ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
}

void GameObject::hit( GameObject *object )
{
	// ����Լ���Ѱ·;
	m_pathFinder.clearPath();

	// ����HP����;
	if (object->m_atk > this->m_hp)
	{
		m_hp = 0.0f;
	}
	else
	{
		m_hp = this->m_hp - object->m_atk;
	}

	// ����HP��ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->updateHPBar();
	view->onHit();
}

void GameObject::die()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Die);

	log("entity die, group %d", this->group());

	// �������Ŀ��;
	this->setAttackTarget(nullptr);

	// ����������׼�ö����Ŀ��;
	std::vector<GameObject *> v;
	for (auto it : this->lockSet())
	{
		auto object = it.second;
		v.push_back(object);
	}
	for (auto object : v)
	{
		object->setAttackTarget(nullptr);
	}

	// ������ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onDie();

	// ���������־;
	m_shouldClean = true;

}

void GameObject::setPosition( const MapPoint &point )
{
	// ����λ������;
	m_position = point;

	// ����λ����ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPosition(point.toPoint());
	view->updateZOrder();
}

void GameObject::setPosition( const Fixed &x, const Fixed &y )
{
	// ����λ������;
	m_position.x = x;
	m_position.y = y;

	// ����λ����ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPosition((float)x, (float)y);
	view->updateZOrder();
}

void GameObject::setPositionX( const Fixed &x )
{
	// ����λ������;
	m_position.x = x;

	// ����λ����ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPositionX((float)x);
}

void GameObject::setPositionY( const Fixed &y )
{
	// ����λ������;
	m_position.y = y;

	// ����λ����ͼ;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPositionY((float)y);
}

void GameObject::setAttackTarget( GameObject *target )
{
	if (m_attackTarget == target)
	{
		// ����Ŀ��û�иı䣬��������;
		return;
	}
	else if (target == this)
	{
		// ���ܹ����Լ�;
		log("cannot set self as attack target");
		CCASSERT(false, "");
		return;
	}
	else if (target && target->group() == this->group()) 
	{
		// ���ܹ���ͬ�飨�Ѿ���;
		log("cannot attack friend");
		CCASSERT(false, "");
		return;
	}
	else
	{
		// ���ԭ�й���Ŀ��;
		if (m_attackTarget)
		{
			m_attackTarget->lockSet().erase(m_id);	
			m_attackTarget = nullptr;
		}

		// �����¹���Ŀ��;
		if (target)
		{
			target->lockSet().insert(std::make_pair(m_id, this));	
			m_attackTarget = target; 
		}
	}
}


