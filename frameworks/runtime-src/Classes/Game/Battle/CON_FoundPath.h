#ifndef CON_FoundPath_h__
#define CON_FoundPath_h__

#include "BehaviorTree/BehaviorTree.h"

class CON_FoundPath : public BTPrecondition
{
public:
	virtual bool onEvaluate(const BTInputParam& input) const;

};

#endif // CON_FoundPath_h__
