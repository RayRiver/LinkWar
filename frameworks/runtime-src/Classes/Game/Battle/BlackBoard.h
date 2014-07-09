#ifndef BlackBoard_h__
#define BlackBoard_h__

#include "cocos2d.h"

class BattleScene;
class GameEntity;
class BlackBoard
{
public:
	BlackBoard();
	~BlackBoard();

	BattleScene *scene;
	GameEntity *self;
};

#endif // BlackBoard_h__
