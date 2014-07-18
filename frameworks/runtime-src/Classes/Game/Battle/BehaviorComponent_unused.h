#ifndef BehaviorComponent_h__
#define BehaviorComponent_h__

#include "cocos2d.h"

class BTNode;
class BlackBoard;
class BehaviorComponent : public cocos2d::Component
{
public:
	static inline const char *name() { return s_name; }

	BehaviorComponent();
	~BehaviorComponent();
	CREATE_FUNC(BehaviorComponent);

	void loadBehavior(const char *filename);
	bool evaluateBehavior(const BlackBoard &inputData);
	void updateBehavior(const BlackBoard &inputData, BlackBoard &outputData);

	inline BTNode *getBehaviorRoot() { return m_root; }

private:
	static const char *s_name;
	BTNode *m_root;
};

#endif // BehaviorComponent_h__
