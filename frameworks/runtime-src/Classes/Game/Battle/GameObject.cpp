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
	, m_direction(GameObjectDirection::Down)
	, m_showHitBox(false)
	, m_behavior(nullptr)
	, m_behaviorIntervalFrames(1)
	, m_currentBehaviorIntervalFrames(0)
	, m_attackTarget(nullptr)
	, m_attackInterval(30)
	, m_currentAttackInterval(m_attackInterval)
	, m_hasFrameMoveTarget(false)
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

	// 初始化对象数据;
	if (!this->initData(config))
	{
		return false;
	}

	// 初始化寻路信息;
	m_pathFinder.init(MAP);

	// 初始化黑板;
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

	// 解析对象属性;
	m_maxhp = Fixed(DICTOOL->getFloatValue_json(dict, "maxhp", 1.0f));
	m_atk = Fixed(DICTOOL->getFloatValue_json(dict, "atk", 0.0f));
	m_hp = m_maxhp;

	// 解析对象行为树;
	const char *behavior_config = DICTOOL->getStringValue_json(dict, "behavior", nullptr);
	if (behavior_config)
	{
		m_behavior = BTReader::getInstance()->readFromFile(behavior_config);
	}

	// 解析行为树更新间隔帧;
	m_behaviorIntervalFrames = DICTOOL->getFloatValue_json(dict, "behavior_interval_frames", m_behaviorIntervalFrames);

	// 速度为每帧的距离;
	m_moveSpeed = DICTOOL->getFloatValue_json(dict, "move_speed", 0.0f);

	// 解析对象受击盒;
	auto hitboxExist = DICTOOL->checkObjectExist_json(dict, "hitbox");
	if (hitboxExist)
	{
		const auto &hitbox = DICTOOL->getSubDictionary_json(dict, "hitbox");
		m_hitBox.x = DICTOOL->getFloatValue_json(hitbox, "x", 0.0f);
		m_hitBox.y = DICTOOL->getFloatValue_json(hitbox, "y", 0.0f);
		m_hitBox.w = DICTOOL->getFloatValue_json(hitbox, "w", 0.0f);
		m_hitBox.h = DICTOOL->getFloatValue_json(hitbox, "h", 0.0f);
	}

	// 解析是否显示受击盒;
	m_showHitBox = DICTOOL->getBooleanValue_json(dict, "show_hitbox", false);

	// 解析对象攻击区域;
	auto attackAreaExist = DICTOOL->checkObjectExist_json(dict, "attack_area");
	if (attackAreaExist)
	{
		const auto &attackArea = DICTOOL->getSubDictionary_json(dict, "attack_area");
		m_attackArea.x = DICTOOL->getFloatValue_json(attackArea, "x", 0.0f);
		m_attackArea.y = DICTOOL->getFloatValue_json(attackArea, "y", 0.0f);
		m_attackArea.w = DICTOOL->getFloatValue_json(attackArea, "w", 0.0f);
		m_attackArea.h = DICTOOL->getFloatValue_json(attackArea, "h", 0.0f);
	}

	// 解析对象动画;
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
	// 更新AI;
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

	/*
	// 初始化 是否想要移动;
	m_desireMove = false;

	// 状态处理;
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
	else 
	*/
	if (m_state == GameObjectState::Attack)
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
	else if (m_state == GameObjectState::MoveToAttack)
	{
		if (this->hasFrameMoveTarget())
		{
			auto mapManager = MapManager::getInstance();
			auto objectManager = GameObjectManager::getInstance();
			auto view = GAME_OBJECT_VIEW(this->id());

			auto pos = this->getPosition();
			auto target = this->getFrameMoveTarget();
			auto &current_grid = mapManager->pos2grid(pos);
			auto target_grid = mapManager->pos2grid(target);
			auto &pathFinder = this->getPathFinder();
			auto moveSpeed = this->getMoveSpeed();

			// 移动到目标位置;
			if (target != pos)
			{
				auto pt = target - pos;
				if (pt.x*pt.x + pt.y*pt.y > moveSpeed*moveSpeed)
				{
					pt.normalize();

					// 特殊地形速度变化;
					if (MAP->getGridType(current_grid.x, current_grid.y) == MapGrid::Type::Mud)
					{
						moveSpeed *= Fixed(0.5);
					}
					else if (MAP->getGridType(current_grid.x, current_grid.y) == MapGrid::Type::Ice)
					{
						moveSpeed *= Fixed(2);
					}

					pt.x *= moveSpeed;
					pt.y *= moveSpeed;
				}

				// 设置朝向;
				auto direction = this->getDirection();
				if (pt.x.abs() > pt.y.abs())
				{
					if (pt.x > Fixed::ZERO)
					{
						this->setDirection(GameObjectDirection::Right);
					}
					else
					{
						this->setDirection(GameObjectDirection::Left);
					}
				}
				else
				{
					if (pt.y > Fixed::ZERO)
					{
						this->setDirection(GameObjectDirection::Up);
					}
					else
					{
						this->setDirection(GameObjectDirection::Down);
					}
				}

				// 朝向改变;
				if (direction != this->getDirection())
				{
					// 播放动画;
					view->onMove();
				}

				// 更新位置;
				pt.x += pos.x;
				pt.y += pos.y;
				this->setPosition(pt);

				// 更新map;
				auto current_grid2 = mapManager->pos2grid(pt);
				if (current_grid != current_grid2)
				{
					mapManager->releaseGrid(current_grid.x, current_grid.y);
					mapManager->retainGrid(current_grid2.x, current_grid2.y);
				}

				if (pt == target)
				{
					// 到达目标地点; 
					this->setHasFrameMoveTarget(false);
				}

			}
		}
	}
}

void GameObject::idle()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Idle);

	// 更新视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onIdle();
}

void GameObject::move( const MapPoint &pos )
{
	m_pathFinder.clearPath();
	this->setMoveTarget(pos);
	this->setState(GameObjectState::Move);

	// 更新视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onMove();
}

void GameObject::moveToAttack()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::MoveToAttack);

	// 更新视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onMove();
}

void GameObject::attack()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Attack);

	m_currentAttackInterval = m_attackInterval;

	// 更新视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
}

void GameObject::hit( GameObject *object )
{
	// 清空自己的寻路;
	m_pathFinder.clearPath();

	// 更新HP数据;
	if (object->m_atk > this->m_hp)
	{
		m_hp = 0.0f;
	}
	else
	{
		m_hp = this->m_hp - object->m_atk;
	}

	// 更新HP视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->updateHPBar();
	view->onHit();
}

void GameObject::die()
{
	m_pathFinder.clearPath();
	this->setState(GameObjectState::Die);

	log("entity die, group %d", this->group());

	// 清楚攻击目标;
	this->setAttackTarget(nullptr);

	// 清理所有瞄准该对象的目标;
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

	// 更新视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->stopAllActions();
	view->onDie();

	// 设置清理标志;
	m_shouldClean = true;

}

void GameObject::setPosition( const MapPoint &point )
{
	// 更新位置数据;
	m_position = point;

	// 更新位置视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPosition(point.toPoint());
	view->updateZOrder();
}

void GameObject::setPosition( const Fixed &x, const Fixed &y )
{
	// 更新位置数据;
	m_position.x = x;
	m_position.y = y;

	// 更新位置视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPosition((float)x, (float)y);
	view->updateZOrder();
}

void GameObject::setPositionX( const Fixed &x )
{
	// 更新位置数据;
	m_position.x = x;

	// 更新位置视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPositionX((float)x);
}

void GameObject::setPositionY( const Fixed &y )
{
	// 更新位置数据;
	m_position.y = y;

	// 更新位置视图;
	auto view = GAME_OBJECT_VIEW(m_id);
	view->setPositionY((float)y);
}

void GameObject::setAttackTarget( GameObject *target )
{
	if (m_attackTarget == target)
	{
		// 攻击目标没有改变，不做处理;
		return;
	}
	else if (target == this)
	{
		// 不能攻击自己;
		log("cannot set self as attack target");
		CCASSERT(false, "");
		return;
	}
	else if (target && target->group() == this->group()) 
	{
		// 不能攻击同组（友军）;
		log("cannot attack friend");
		CCASSERT(false, "");
		return;
	}
	else
	{
		// 清除原有攻击目标;
		if (m_attackTarget)
		{
			m_attackTarget->lockSet().erase(m_id);	
			m_attackTarget = nullptr;
		}

		// 设置新攻击目标;
		if (target)
		{
			target->lockSet().insert(std::make_pair(m_id, this));	
			m_attackTarget = target; 
		}
	}
}


