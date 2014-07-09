#ifndef BTNodeAction_h__
#define BTNodeAction_h__

#include "BTNode.h"

class BTNodeAction : public BTNode
{
public:
	BTNodeAction(BTPrecondition *precondition = nullptr);

protected:
	inline bool isRunning() { return m_state == BTActionStatus::Running; }
	inline void finish() { m_state = BTActionStatus::Finish; }

	// override these functions
protected:
	virtual void onEnter(const BTInputParam &input);
	virtual void onExit(const BTInputParam &input, BTRunningStatus state);
	virtual BTRunningStatus onExecute(const BTInputParam &input, BTOutputParam &output);

protected:
	virtual void onTransition(const BTInputParam &input);
	virtual BTRunningStatus onUpdate(const BTInputParam &input, BTOutputParam &output);

private:
	void doEnter(const BTInputParam &input);
	void doExit(const BTInputParam &input, BTRunningStatus state);

private:
	//bool m_isRunning;
	BTActionStatus m_state;
	bool m_bNeedExit;
};

#endif // BTNodeAction_h__
