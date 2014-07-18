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

		// ���ٽ����Զ��ͷųع���;
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
	// ���ö����Ƿ����;
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		log("createObject: id=%d, object already exist", id);
		return it->second.object;	
	}

	// ����ʵ��;
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

	// ������ͼ;
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

	// ������Ӧ��ϵ;
	if (object && view)
	{
		this->addChild(view);

		ObjectData data;
		data.object = object;
		data.view = view;

		m_objectMap.insert(ObjectMapPair(id, data));
	}

	// ��ʼ���ɹ�����ʾ;
	view->onInitialized();

	return object;
}

void GameObjectManager::destroyObject( int id )
{
	// ���ö����Ƿ����;
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		auto &data = it->second;

		// ���ٶ���;
		if (data.object)
		{
			delete data.object;
			data.object = nullptr;
		}

		// ɾ����¼;
		if (!data.object && !data.view)
		{
			m_objectMap.erase(it);
		}
	}
	else
	{
		log("destroyObject: id=%d, object not exist", id);
	}
}

void GameObjectManager::destroyObjectView( int id )
{
	// ���ö����Ƿ����;
	auto it = m_objectMap.find(id);
	if (it != m_objectMap.end())
	{
		auto &data = it->second;

		// ���ٶ�����ͼ;
		if (data.view)
		{
			this->removeChild(data.view);
			data.view = nullptr;
		}

		// ɾ����¼;
		if (!data.object && !data.view)
		{
			m_objectMap.erase(it);
		}
	}
	else
	{
		log("destroyObjectView: id=%d, object view not exist", id);
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
		bool ret = callback(id, data.object, data.view);
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
			bool ret = callback(id, data.object, data.view);
			if (!ret)
			{
				break;
			}
		}
	}
}



