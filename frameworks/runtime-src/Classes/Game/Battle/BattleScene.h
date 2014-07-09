#ifndef BattleScene_h__
#define BattleScene_h__

#include <set>

#include "cocos2d.h"

class BTNode;
class GameEntity;
class BattleScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene *createScene();
	virtual bool init();
	CREATE_FUNC(BattleScene);

	virtual void update(float dt);

	void createBTDebugRenderer(BTNode *node);
	void createGrid();
	void createLauncherArea();
	void createBattleFieldArea();
	GameEntity *createEntity(bool isEnemy, const cocos2d::Vec2 &pos);

	inline const cocos2d::Rect &getBattleFieldArea() { return m_battleFieldArea; }
	inline const std::set<GameEntity *> &getSelfSoldiers() { return m_selfSoldiers; }
	inline const std::set<GameEntity *> &getOppoSoldiers() { return m_oppoSoldiers; }

private:
	cocos2d::DrawNode *m_drawNode;
	cocos2d::Rect m_selfLauncherArea;
	cocos2d::Rect m_oppoLauncherArea;
	cocos2d::Rect m_battleFieldArea;

	std::set<GameEntity *> m_selfSoldiers;
	std::set<GameEntity *> m_oppoSoldiers;

};

#endif // BattleScene_h__
