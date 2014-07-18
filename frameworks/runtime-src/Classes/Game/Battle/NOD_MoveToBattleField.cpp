#include "NOD_MoveToBattleField.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"
#include "MapManager.h"
#include "GameObjectManager.h"

USING_NS_CC;

void NOD_MoveToBattleField::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto self = inputData.self;

	const auto &rect = MAP->getBattleFieldArea();
	const auto &pos = self->getPosition();
	const auto &hitbox = self->getHitBox();

	MapPoint target;
	target.x = pos.x;
	if (self->group() == (int)GameObjectGroup::Group0)
	{
		target.y = rect.y + hitbox.h/Fixed(2);
	}
	else
	{
		target.y = rect.y + rect.h - hitbox.h/Fixed(2);
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

	auto self = inputData.self;

	if (self->getPosition() == self->getMoveTarget())
	{
		self->setProperty("isReachedBattleField", Fixed::ONE);
		return BTRunningStatus::Finish;
	}
	else
	{
		return BTRunningStatus::Executing;
	}
}

BT_REGISTER_ACTION_NODE(NOD_MoveToBattleField);
