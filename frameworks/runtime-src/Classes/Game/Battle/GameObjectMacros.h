#ifndef GameObjectMacros_h__
#define GameObjectMacros_h__

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

enum class GameObjectDirection
{
	Left,
	Right,
	Up,
	Down,
};

#endif // GameObjectMacros_h__
