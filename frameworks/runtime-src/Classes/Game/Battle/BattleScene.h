#ifndef BattleScene_h__
#define BattleScene_h__

#include <set>

#include "cocos2d.h"

class BTNode;
class GameEntity;
class BattleScene : public cocos2d::Scene
{
public:
	BattleScene();
	~BattleScene();

	static cocos2d::Scene *createScene();
	virtual bool init();
	CREATE_FUNC(BattleScene);

	virtual void update(float dt);

	void createBTDebugRenderer(BTNode *node);
	void createGrid();
	void createLauncherArea();
	void createBattleFieldArea();
	void createMap();
	GameEntity *createEntity(bool isEnemy, const cocos2d::Vec2 &pos);

	inline const cocos2d::Rect &getBattleFieldArea() { return m_battleFieldArea; }
	inline const std::set<GameEntity *> &getSelfSoldiers() { return m_selfSoldiers; }
	inline const std::set<GameEntity *> &getOppoSoldiers() { return m_oppoSoldiers; }

	inline unsigned char *getMap() { return m_map; }
	inline int getMapW() { return m_mapW; }
	inline int getMapH() { return m_mapH; }
	void mapNodeRetain(int x, int y);
	void mapNodeRelease(int x, int y);
	int mapNodeRef(int x, int y);

	int getGridSize();

private:
	cocos2d::DrawNode *m_drawNode;
	cocos2d::Rect m_selfLauncherArea;
	cocos2d::Rect m_oppoLauncherArea;
	cocos2d::Rect m_battleFieldArea;

	std::set<GameEntity *> m_selfSoldiers;
	std::set<GameEntity *> m_oppoSoldiers;

	unsigned char *m_map;
	int m_mapW;
	int m_mapH;

	cocos2d::DrawNode *m_debugDrawNode;
};

#endif // BattleScene_h__
