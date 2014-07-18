#include "CON_ReachedBattleField.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"

USING_NS_CC;

bool CON_ReachedBattleField::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto self = inputData.self;

	Fixed val;
	if (self->getProperty("isReachedBattleField", val))
	{
		return val!=Fixed::ZERO;
	}
	else
	{
		return false;
	}
}

BT_REGISTER_PRECONDITION(CON_ReachedBattleField)
