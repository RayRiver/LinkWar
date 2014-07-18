#ifndef GameObjectState_h__
#define GameObjectState_h__

enum class GameObjectState
{
	Idle,
	Move,
	MoveToAttack,
	Attack,
	Die,
};

enum class GameObjectGroup
{
	Group0 = 0,	
	Group1,
};

#endif // GameObjectState_h__
