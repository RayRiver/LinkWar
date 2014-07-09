#ifndef CON_ReachedBattleField_h__
#define CON_ReachedBattleField_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_ReachedBattleField : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_ReachedBattleField_h__
