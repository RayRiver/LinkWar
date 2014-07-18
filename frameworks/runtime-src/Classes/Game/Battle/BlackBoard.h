#ifndef BlackBoard_h__
#define BlackBoard_h__

#include "cocos2d.h"

class GameObject;
class BlackBoard
{
public:
	BlackBoard();
	~BlackBoard();

	GameObject *self;
};

#endif // BlackBoard_h__
