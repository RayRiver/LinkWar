#include "CON_HP0.h"

#include "cocos2d.h"
#include "CCLuaValue.h"

#include "BlackBoard.h"
#include "BattleScene.h"
#include "GameEntity.h"
#include "Helper/Display.h"

USING_NS_CC;

bool CON_HP0::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto scene = inputData.scene;
	auto self = inputData.self;

	LuaValue val;
	if (self->getProperty("hp", val))
	{
		if (val.floatValue() == 0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

BT_REGISTER_PRECONDITION(CON_HP0)
