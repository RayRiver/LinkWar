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

	auto pos = self->getPosition();
	float distance2 = 0.0f;

	// find attack target
	GameEntity *foundEntity = nullptr;
	if (self->isEnemy())
	{
		for (const auto &entity : scene->getSelfSoldiers())
		{
			if (!entity->shouldClean())
			{
				float d2 = (entity->getPosition() - pos).lengthSquared();
				if (!foundEntity || d2 < distance2)
				{
					distance2 = d2;
					foundEntity = entity;
				}
			}
		}	
	}
	else
	{
		for (const auto &entity : scene->getOppoSoldiers())
		{
			if (!entity->shouldClean())
			{
				float d2 = (entity->getPosition() - pos).lengthSquared();
				if (!foundEntity || d2 < distance2)
				{
					distance2 = d2;
					foundEntity = entity;
				};
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
