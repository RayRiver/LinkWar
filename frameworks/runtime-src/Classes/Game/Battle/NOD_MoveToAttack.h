#ifndef NOD_MoveToAttack_h__
#define NOD_MoveToAttack_h__

#include "BehaviorTree/BehaviorTree.h"

class NOD_MoveToAttack : public BTNodeAction
{
protected:
	virtual void onEnter(const BTInputParam &input);
	virtual void onExit(const BTInputParam &input, BTRunningStatus state);
	virtual BTRunningStatus onExecute(const BTInputParam& input, BTOutputParam& output);
};

#endif // NOD_MoveToAttack_h__
