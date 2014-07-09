#ifndef CON_HP0_h__
#define CON_HP0_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_HP0 : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_HP0_h__
