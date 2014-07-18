#include "GameObjectData.h"

#include "cocos2d.h"
#include "CCLuaValue.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

USING_NS_CC;
using namespace cocostudio;

const std::string EMPTY_STRING = "";
const Fixed EMPTY_FIXED = Fixed();

GameObjectData::GameObjectData()
	: m_shouldClean(false)
	, m_id(-1)
	, m_group(-1)
	, m_isEnemy(false)
	, m_state(GameObject::State::Idle)
	, m_desireMove(false)
	, m_behavior(nullptr)
	, m_behaviorIntervalFrames(1)
	, m_currentBehaviorIntervalFrames(0)
	, m_attackTarget(nullptr)
	, m_attackInterval(30)
	, m_currentAttackInterval(m_attackInterval)
{

}

GameObjectData::~GameObjectData()
{
	if (m_behavior)
	{
		delete m_behavior;
		m_behavior = nullptr;
	}
}

bool GameObjectData::init( const char *config, int id, int group )
{
	m_id = id;
	m_group = group;

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(config);
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	auto doc = rapidjson::Document();
	doc.Parse<0>(fileContent.c_str());
	if (doc.HasParseError()) {
		CCLOG("GetParseError %s\n", doc.GetParseError());
		return false;
	}

	const rapidjson::Value &dict = doc;

	// parse properties
	m_maxhp = Fixed(DICTOOL->getFloatValue_json(dict, "maxhp", 1.0f));
	m_atk = Fixed(DICTOOL->getFloatValue_json(dict, "atk", 0.0f));
	m_hp = m_maxhp;

	// parse move speed
	m_moveSpeed = DICTOOL->getFloatValue_json(dict, "move_speed", 0.0f);

	// parse behavior tree
	const char *behavior_config = DICTOOL->getStringValue_json(dict, "behavior", nullptr);
	if (behavior_config)
	{
		m_behavior = BTReader::getInstance()->readFromFile(behavior_config);
	}

	// parse behavior interval
	m_behaviorIntervalFrames = DICTOOL->getFloatValue_json(dict, "behavior_interval_frames", m_behaviorIntervalFrames);

	// parse hit box
	auto hitboxExist = DICTOOL->checkObjectExist_json(dict, "hitbox");
	if (hitboxExist)
	{
		const auto &hitbox = DICTOOL->getSubDictionary_json(dict, "hitbox");
		m_hitBox.x = DICTOOL->getFloatValue_json(hitbox, "x", 0.0f);
		m_hitBox.y = DICTOOL->getFloatValue_json(hitbox, "y", 0.0f);
		m_hitBox.w = DICTOOL->getFloatValue_json(hitbox, "w", 0.0f);
		m_hitBox.h = DICTOOL->getFloatValue_json(hitbox, "h", 0.0f);
	}

	// parse attack area
	auto attackAreaExist = DICTOOL->checkObjectExist_json(dict, "attack_area");
	if (attackAreaExist)
	{
		const auto &attackArea = DICTOOL->getSubDictionary_json(dict, "attack_area");
		m_attackArea.x = DICTOOL->getFloatValue_json(attackArea, "x", 0.0f);
		m_attackArea.y = DICTOOL->getFloatValue_json(attackArea, "y", 0.0f);
		m_attackArea.w = DICTOOL->getFloatValue_json(attackArea, "w", 0.0f);
		m_attackArea.h = DICTOOL->getFloatValue_json(attackArea, "h", 0.0f);
	}

	return true;
}

void GameObjectData::removeProperty( const char *name )
{
	auto it = m_propertyMap.find(name);
	if (it == m_propertyMap.end())
	{
		log("game entity remove property [%s]: no such property", name);
	}
	else
	{
		m_propertyMap.erase(it);
	}
}

void GameObjectData::setProperty( const char *name, const Fixed &val )
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

bool GameObjectData::getProperty( const char *name, Fixed &val )
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

void GameObjectData::setProperty( const char *name, const char *val )
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

bool GameObjectData::getProperty( const char *name, std::string &val )
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

