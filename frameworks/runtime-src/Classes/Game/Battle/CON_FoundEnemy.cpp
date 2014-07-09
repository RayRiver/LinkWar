#include "CON_FoundEnemy.h"

#include "BlackBoard.h"
#include "BattleScene.h"
#include "GameEntity.h"
#include "Helper/Display.h"

bool CON_FoundEnemy::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto scene = inputData.scene;
	auto self = inputData.self;

	// find attack target
	GameEntity *foundEntity = nullptr;
	if (self->isEnemy())
	{
		float y = 0.0f;
		for (const auto &entity : scene->getSelfSoldiers())
		{
			if (!entity->shouldClean() && y < entity->getPositionY())
			{
				y = entity->getPositionY();
				foundEntity = entity;
			}
		}	
	}
	else
	{
		float y = display.top();
		for (const auto &entity : scene->getOppoSoldiers())
		{
			if (!entity->shouldClean() && y > entity->getPositionY())
			{
				y = entity->getPositionY();
				foundEntity = entity;
			}
		}	
	}

	if (foundEntity)
	{
		self->setAttackTarget(foundEntity);
		return true;
	}
	else
	{
		return false;
	}
}

BT_REGISTER_PRECONDITION(CON_FoundEnemy)
