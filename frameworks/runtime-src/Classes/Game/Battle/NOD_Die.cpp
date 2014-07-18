#include "NOD_Die.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"

USING_NS_CC;

void NOD_Die::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto self = inputData.self;

	self->die();
}

void NOD_Die::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_Die::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	return BTRunningStatus::Executing;
}

BT_REGISTER_ACTION_NODE(NOD_Die);
