#ifndef NOD_Die_h__
#define NOD_Die_h__

#include "BehaviorTree/BehaviorTree.h"

class NOD_Die : public BTNodeAction
{
protected:
	virtual void onEnter(const BTInputParam &input);
	virtual void onExit(const BTInputParam &input, BTRunningStatus state);
	virtual BTRunningStatus onExecute(const BTInputParam& input, BTOutputParam& output);
};

#endif // NOD_Die_h__
