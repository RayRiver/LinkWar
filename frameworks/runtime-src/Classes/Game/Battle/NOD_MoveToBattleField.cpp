#include "NOD_MoveToBattleField.h"

#include "cocos2d.h"
#include "CCLuaValue.h"

#include "BlackBoard.h"
#include "GameEntity.h"
#include "BattleScene.h"

USING_NS_CC;

void NOD_MoveToBattleField::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto scene = inputData.scene;
	auto self = inputData.self;

	const auto &rect = scene->getBattleFieldArea();
	const auto &pos = self->getPosition();
	const auto &hitbox = self->getHitBox();

	Vec2 target;
	target.x = pos.x;
	if (self->isEnemy())
	{
		target.y = rect.origin.y + rect.size.height - hitbox.size.height/2;
	}
	else
	{
		target.y = rect.origin.y + hitbox.size.height/2;
	}

	self->move(target);
}

void NOD_MoveToBattleField::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_MoveToBattleField::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	auto scene = inputData.scene;
	auto self = inputData.self;

	if (self->getPosition() == self->getMoveTarget())
	{
		self->setProperty("isReachedBattleField", LuaValue::booleanValue(true));
		return BTRunningStatus::Finish;
	}
	else
	{
		return BTRunningStatus::Executing;
	}
}

BT_REGISTER_ACTION_NODE(NOD_MoveToBattleField);
