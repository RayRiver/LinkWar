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

	// ��������;
	GameObject *createObject(int id, int type, int group);

	// ���ٶ���;
	void destroyObject(int id);
	void destroyObjectView(int id);

	// ��ȡ����;
	GameObject *getObject(int id);

	// ��ȡ������ͼ;
	GameObjectView *getObjectView(int id);

	// ����ִ��;
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
