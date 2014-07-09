#ifndef CON_FoundEnemy_h__
#define CON_FoundEnemy_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_FoundEnemy : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_FoundEnemy_h__
