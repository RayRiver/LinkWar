#include "NOD_Idle.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"

USING_NS_CC;

void NOD_Idle::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto self = inputData.self;

	self->idle();
	this->finish();
	/*
	self->runAction(Sequence::create(DelayTime::create(1.0f), CallFunc::create([=]() {
		this->finish();
	}), nullptr));
	*/
}

void NOD_Idle::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_Idle::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	return BTRunningStatus::Executing;
}

BT_REGISTER_ACTION_NODE(NOD_Idle);
