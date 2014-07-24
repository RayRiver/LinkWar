#include "NOD_MoveToAttack.h"

#include "BlackBoard.h"
#include "GameObject.h"
#include "GameObjectView.h"
#include "GameObjectManager.h"

void NOD_MoveToAttack::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();



}

void NOD_MoveToAttack::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_MoveToAttack::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	auto object = inputData.self;

	return BTRunningStatus::Executing;

	if (object->hasFrameMoveTarget())
	{
		return BTRunningStatus::Executing;
	}
	else
	{
		return BTRunningStatus::Finish;
	}

}

BT_REGISTER_ACTION_NODE(NOD_MoveToAttack);
