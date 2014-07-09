#include "GameEntity.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

#include "BehaviorComponent.h"
#include "BattleScene.h"
#include "Helper/Display.h"

USING_NS_CC;
using namespace cocostudio;

const int RECT_POINTS = 4;

GameEntity * GameEntity::create( const char *filename, BattleScene *scene, bool isEnemy )
{
	auto ret = new GameEntity;
	if (ret && ret->init(filename, scene, isEnemy))
	{
		ret->autorelease();
	}
	else
	{
		delete ret;
		ret = nullptr;
	}
	return ret;
}

bool GameEntity::init( const char *filename, BattleScene *scene, bool isEnemy )
{
	if (!Node::init())
	{
		return false;
	}

	this->setAnchorPoint(Vec2(0.5f, 0.5f));

	m_hitBox = Rect::ZERO;
	m_isEnemy = isEnemy;

	m_blackboard.scene = scene;
	m_blackboard.self = this;

	m_state = State::Idle;

	m_moveSpeed = 0.0f;

	m_attackArea = Rect::ZERO;
	m_behaviorInterval = 1.0f;

	m_bShouldClean = false;

	m_currentBehaviorInterval = 0.0f;

	m_attackTarget = nullptr;

	if (!this->loadFromFile(filename))
	{
		return false;
	}

	//this->schedule(schedule_selector(GameEntity::onEvaluate), m_behaviorInterval);

	return true;
}

void GameEntity::onFrame( float dt )
{
	m_currentBehaviorInterval += dt;
	if (m_currentBehaviorInterval > m_behaviorInterval)
	{
		this->onEvaluate(m_currentBehaviorInterval);
		m_currentBehaviorInterval -= m_behaviorInterval;
	}

	if (m_state == State::Move)
	{
		auto pos = getPosition();
		if (pos != m_moveTarget)
		{
			auto pt = m_moveTarget - pos;
			if (pt.lengthSquared() > 1.0f)
			{
				pt.normalize();
				pt *= m_moveSpeed;
			}
			pt.x += pos.x;
			pt.y += pos.y;
			this->setPosition(pt);
			this->updateZOrder();	
		}
		else
		{
			m_state = State::Idle;
		}
	}
	else if (m_state == State::MoveToAttack)
	{
		auto pos = getPosition();
		if (this->getAttackTarget())
		{
			const auto &moveTarget = this->getAttackTarget()->getPosition();
			if (pos != moveTarget)
			{
				auto pt = moveTarget - pos;
				if (pt.lengthSquared() > 1.0f)
				{
					pt.normalize();
					pt *= m_moveSpeed;
				}
				pt.x += pos.x;
				pt.y += pos.y;
				this->setPosition(pt);
				this->updateZOrder();	
			}
		}
	}
	else if (m_state == State::Attack)
	{

	}

}

void GameEntity::onEvaluate( float dt )
{
	auto behavior = dynamic_cast<BehaviorComponent *>(this->getComponent(BehaviorComponent::name()));
	if (behavior && behavior->evaluateBehavior(m_blackboard))
	{
		behavior->updateBehavior(m_blackboard, m_blackboard);
	}
}

void GameEntity::addProperty( const char *name, const cocos2d::LuaValue &val )
{
	const auto &it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		m_propertyMap.insert(PROPERTY_MAP_PAIR(name, val));
	}
	else
	{
		log("game entity add property [%s]: already exist", name);
		m_propertyMap[name] = val;
	}
}

void GameEntity::setProperty( const char *name, const cocos2d::LuaValue &val )
{
	const auto &it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		m_propertyMap.insert(PROPERTY_MAP_PAIR(name, val));
	}
	else
	{
		m_propertyMap[name] = val;
	}
}

void GameEntity::removeProperty( const char *name )
{
	const auto &it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		log("game entity remove property [%s]: no such property", name);
	}
	else
	{
		m_propertyMap.erase(it);
	}
}

bool GameEntity::getProperty( const char *name, cocos2d::LuaValue &val )
{
	const auto &it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		log("game entity get property [%s]: no such property", name);
		return false;
	}
	else
	{
		val = it->second;
		return true;
	}
}

bool GameEntity::loadFromFile( const char *filename )
{
	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	auto doc = rapidjson::Document();
	doc.Parse<0>(fileContent.c_str());
	if (doc.HasParseError()) {
		CCLOG("GetParseError %s\n", doc.GetParseError());
		return false;
	}

	const rapidjson::Value &dict = doc;

	// parse properties
	auto maxhp = DICTOOL->getFloatValue_json(dict, "maxhp", 1.0f);
	auto atk = DICTOOL->getFloatValue_json(dict, "atk", 0.0f);
	this->setProperty("maxhp", LuaValue::floatValue(maxhp));
	this->setProperty("hp", LuaValue::floatValue(maxhp));
	this->setProperty("atk", LuaValue::floatValue(atk));

	// parse move speed
	m_moveSpeed = DICTOOL->getFloatValue_json(dict, "move_speed", 0.0f);

	// parse behavior tree
	const char *behavior_config = DICTOOL->getStringValue_json(dict, "behavior", nullptr);
	if (behavior_config)
	{
		auto behavior = BehaviorComponent::create();
		behavior->loadBehavior(behavior_config);
		this->addComponent(behavior);
		m_blackboard.scene->createBTDebugRenderer(behavior->getBehaviorRoot());
	}

	// parse behavior interval
	m_behaviorInterval = DICTOOL->getFloatValue_json(dict, "behavior_interval", m_behaviorInterval);

	// parse hit box
	auto hitboxExist = DICTOOL->checkObjectExist_json(dict, "hitbox");
	if (hitboxExist)
	{
		const auto &hitbox = DICTOOL->getSubDictionary_json(dict, "hitbox");
		m_hitBox.origin.x = DICTOOL->getFloatValue_json(hitbox, "x", 0.0f);
		m_hitBox.origin.y = DICTOOL->getFloatValue_json(hitbox, "y", 0.0f);
		m_hitBox.size.width = DICTOOL->getFloatValue_json(hitbox, "w", 0.0f);
		m_hitBox.size.height = DICTOOL->getFloatValue_json(hitbox, "h", 0.0f);

		// show hit box
		auto drawNode = DrawNode::create();
		this->addChild(drawNode);
		if (drawNode)
		{
			Vec2 verts[RECT_POINTS];
			display.rect2points(m_hitBox, verts);
			if (m_isEnemy)
			{
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}

	// parse attack area
	auto attackAreaExist = DICTOOL->checkObjectExist_json(dict, "attack_area");
	if (attackAreaExist)
	{
		const auto &attackArea = DICTOOL->getSubDictionary_json(dict, "attack_area");
		m_attackArea.origin.x = DICTOOL->getFloatValue_json(attackArea, "x", 0.0f);
		m_attackArea.origin.y = DICTOOL->getFloatValue_json(attackArea, "y", 0.0f);
		m_attackArea.size.width = DICTOOL->getFloatValue_json(attackArea, "w", 0.0f);
		m_attackArea.size.height = DICTOOL->getFloatValue_json(attackArea, "h", 0.0f);

		// show attack area
		auto drawNode = DrawNode::create();
		this->addChild(drawNode);
		if (drawNode)
		{
			Vec2 verts[RECT_POINTS];
			display.rect2points(m_attackArea, verts);
			if (m_isEnemy)
			{
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 0.2f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.5f));
			}
			else
			{
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 0.2f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.5f));
			}
		}
	}

	// show hp bar
	m_hpBar = DrawNode::create();
	this->addChild(m_hpBar);
	this->updateHPBar();

	return true;
}

void GameEntity::updateZOrder()
{
	this->setLocalZOrder(10000 + (int)(display.height() - this->getPositionY()));
}


void GameEntity::idle()
{
	this->stopAllActions();
	this->setState(GameEntity::State::Idle);
}

void GameEntity::move( const cocos2d::Vec2 &pos )
{
	this->stopAllActions();
	this->setMoveTarget(pos);
	this->setState(GameEntity::State::Move);
}

void GameEntity::moveToAttack()
{
	this->stopAllActions();
	this->setState(GameEntity::State::MoveToAttack);
}

void GameEntity::attack()
{
	this->stopAllActions();
	this->setState(GameEntity::State::Attack);

	this->runAction(RepeatForever::create(Sequence::create(
		DelayTime::create(0.5f),	
		CallFunc::create([=](){
			auto target = getAttackTarget();
			if (target)
			{
				target->hit(this);
			}
		}),
		DelayTime::create(0.5f),	
		nullptr)));
}

void GameEntity::hit( GameEntity * entity )
{
	LuaValue val;
	float atk = 0.0f;
	float hp = 1.0f;

	if (entity->getProperty("atk", val))
	{
		atk = val.floatValue();	
	}
	if (this->getProperty("hp", val))
	{
		hp = val.floatValue();
	}

	if (atk > hp)
	{
		this->setProperty("hp", LuaValue::floatValue(0.0f));
	}
	else
	{
		this->setProperty("hp", LuaValue::floatValue(hp-atk));
	}

	this->updateHPBar();
}

void GameEntity::die()
{
	this->stopAllActions();
	this->setState(GameEntity::State::Die);

	log("entity die %s", this->isEnemy() ? "enemy" : "self");


	this->setAttackTarget(nullptr);

	// 清理所有瞄准该对象的目标
	std::vector<GameEntity *> v;
	for (auto entity : m_lockSet)
	{
		v.push_back(entity);
	}
	for (auto entity : v)
	{
		entity->setAttackTarget(nullptr);
	}

	m_bShouldClean = true;

	/*
	this->runAction(Sequence::create(DelayTime::create(1.0f), Blink::create(1.0f, 2), CallFunc::create([=]() {

	}), nullptr));
	*/
}

void GameEntity::updateHPBar()
{
	float hp = 1.0f;
	float maxhp = 1.0f;

	LuaValue val;
	if (this->getProperty("hp", val))
	{
		hp = val.floatValue();			
	}
	if (this->getProperty("maxhp", val))
	{
		maxhp = val.floatValue();
	}

	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(20, 30), 5.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(20, 30), 4.0f, Color4F(0.0f, 0.0f, 0.0f, 1.0f));
	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(-20+40*(hp/maxhp), 30), 4.0f, Color4F(1.0f, 0.0f, 0.0f, 1.0f));
}

void GameEntity::setAttackTarget( GameEntity *target )
{
	if (m_attackTarget == target)
	{
		return;
	}
	else if (target == this)
	{
		log("cannot set self as attack target");
		CCASSERT(false, "");
		return;
	}
	else if (target && target->isEnemy() == this->isEnemy()) 
	{
		log("cannot attack friend");
		CCASSERT(false, "");
		return;
	}
	else
	{
		if (m_attackTarget)
		{
			m_attackTarget->lockSet().erase(this);	
			log("0x%08x cancel lock on 0x%08x", this, m_attackTarget);
			m_attackTarget = nullptr;
		}

		if (target)
		{
			target->lockSet().insert(this);
			m_attackTarget = target; 
			log("0x%08x lock on 0x%08x", this, target);
		}
	}

}

