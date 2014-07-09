#include "NOD_Attack.h"

#include "BlackBoard.h"
#include "GameEntity.h"

void NOD_Attack::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto self = inputData.self;

	auto target = self->getAttackTarget();
	if (target)
	{
		self->attack();
	}

}

void NOD_Attack::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_Attack::onExecute( const BTInputParam& input, BTOutputParam& output )
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

BT_REGISTER_ACTION_NODE(NOD_Attack);
