#include "CON_HP0.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"

USING_NS_CC;

bool CON_HP0::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto self = inputData.self;

	if (self->hp() == Fixed::ZERO)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BT_REGISTER_PRECONDITION(CON_HP0)
