#include "NOD_MoveToAttack.h"

#include "BlackBoard.h"
#include "GameEntity.h"

void NOD_MoveToAttack::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto self = inputData.self;

	auto target = self->getAttackTarget();
	if (target)
	{
		self->moveToAttack();
	}

}

void NOD_MoveToAttack::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_MoveToAttack::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	auto self = inputData.self;
	if (self->getAttackTarget())
	{
		return BTRunningStatus::Executing;
	}
	else
	{
		return BTRunningStatus::Finish;
	}
}

BT_REGISTER_ACTION_NODE(NOD_MoveToAttack);
