#ifndef BlackBoard_h__
#define BlackBoard_h__

#include "cocos2d.h"

class MapManager;
class GameEntity;
class BlackBoard
{
public:
	BlackBoard();
	~BlackBoard();

	MapManager *mapManager;
	GameEntity *self;
};

#endif // BlackBoard_h__
