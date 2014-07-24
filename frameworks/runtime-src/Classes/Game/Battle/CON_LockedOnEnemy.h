#ifndef CON_LockedOnEnemy_h__
#define CON_LockedOnEnemy_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_LockedOnEnemy : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_LockedOnEnemy_h__
