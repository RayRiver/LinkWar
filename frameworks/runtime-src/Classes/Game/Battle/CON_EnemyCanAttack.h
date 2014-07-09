#ifndef CON_EnemyCanAttack_h__
#define CON_EnemyCanAttack_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_EnemyCanAttack : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_EnemyCanAttack_h__
