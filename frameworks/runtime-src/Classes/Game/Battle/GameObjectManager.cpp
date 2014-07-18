#include "GameObjectManager.h"

#include "GameObject.h"
#include "GameObjectView.h"

#include "MapManager.h"

USING_NS_CC;

GameObjectManager * GameObjectManager::s_instance = nullptr;

GameObjectManager * GameObjectManager::getInstance()
{
	if (!s_instance)
	{
		s_instance = new GameObjectManager;
		s_instance->init();

		// 不再交由自动释放池管理;
	}
	return s_instance;
}

void GameObjectManager::destroyInstance()
{
	delete s_instance;
	s_instance = nullptr;
}

GameObjectManager::GameObjectManager()
{

}

GameObjectManager::~GameObjectManager()
{

}

GameObject * GameObjectManager::createObject( int id, int type, int group )
{
	// 检查该对象是否存在;
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		log("createObject: id=%d, object already exist", id);
		return it->second.object;	
	}

	// 创建实体;
	char config[128];
	sprintf(config, "config/gameobject_%d.json", type);
	auto object = new GameObject;
	if (object && object->init(config, id, group))
	{

	}
	else
	{
		delete object;
		object = nullptr;
		return nullptr;
	}

	// 创建视图;
	auto view = new GameObjectView;
	if (view && view->init(id))
	{
		view->autorelease();
	}
	else
	{
		delete view;
		view = nullptr;

		delete object;
		object = nullptr;
	}

	// 创建对应关系;
	if (object && view)
	{
		this->addChild(view);

		ObjectData data;
		data.object = object;
		data.view = view;

		m_objectMap.insert(ObjectMapPair(id, data));
	}

	// 初始化成功，显示;
	view->onInitialized();

	return object;
}

void GameObjectManager::destroyObject( int id )
{
	// 检查该对象是否存在;
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		auto object = it->second.object;
		auto view = it->second.view;
		m_objectMap.erase(it);

		// 销毁对象;
		delete object;

		// 销毁对象视图;
		this->removeChild(view);
	}
	else
	{
		log("destroyObject: id=%d, object not exist", id);
	}
}

GameObject * GameObjectManager::getObject( int id )
{
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		return it->second.object;	
	}
	else
	{
		return nullptr;
	}
}

GameObjectView * GameObjectManager::getObjectView( int id )
{
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		return it->second.view;	
	}
	else
	{
		return nullptr;
	}
}

void GameObjectManager::callObjects( const ObjectCallback &callback )
{
	for (auto it : m_objectMap)
	{
		auto id = it.first;
		const auto &data = it.second;
		bool ret = callback(data.object, data.view);
		if (!ret)
		{
			break;
		}
	}
}

void GameObjectManager::callObjectsInGroup( GameObjectGroup group, const ObjectCallback &callback )
{
	for (auto it : m_objectMap)
	{
		auto id = it.first;
		const auto &data = it.second;
		if (data.object->group() == (int)group)
		{
			bool ret = callback(data.object, data.view);
			if (!ret)
			{
				break;
			}
		}
	}
}



