#ifndef GameObjectManager_h__
#define GameObjectManager_h__

#include <map>

#include "cocos2d.h"

#include "GameObjectMacros.h"

class GameObject;
class GameObjectView;

class GameObjectManager : cocos2d::Node
{
public:
	typedef std::function<bool(int, GameObject *, GameObjectView *)> ObjectCallback;

public:
	static GameObjectManager *getInstance();
	static void destroyInstance();
	~GameObjectManager();

	// 创建对象;
	GameObject *createObject(int id, int type, int group);

	// 销毁对象;
	void destroyObject(int id);
	void destroyObjectView(int id);

	// 获取对象;
	GameObject *getObject(int id);

	// 获取对象视图;
	GameObjectView *getObjectView(int id);

	// 遍历执行;
	void callObjects(const ObjectCallback &callback);
	void callObjectsInGroup(GameObjectGroup group, const ObjectCallback &callback);

private:
	GameObjectManager();
	static GameObjectManager *s_instance;
	
private:
	struct ObjectData 
	{
		GameObject *object;
		GameObjectView *view;
	};
	typedef std::map<int, ObjectData> ObjectMap;
	typedef std::pair<int, ObjectData> ObjectMapPair;

	ObjectMap m_objectMap;

};

#define OBJECTS GameObjectManager::getInstance()
#define GAME_OBJECT(id) GameObjectManager::getInstance()->getObject(id)
#define GAME_OBJECT_VIEW(id) GameObjectManager::getInstance()->getObjectView(id)

#endif // GameObjectManager_h__
