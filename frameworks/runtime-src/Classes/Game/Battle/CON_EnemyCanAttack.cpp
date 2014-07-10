#include "CON_EnemyCanAttack.h"

#include "BlackBoard.h"
#include "BattleScene.h"
#include "GameEntity.h"

bool CON_EnemyCanAttack::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto scene = inputData.scene;
	auto self = inputData.self;

	auto area = self->getAttackArea();
	area.origin.x += self->getPositionX();
	area.origin.y += self->getPositionY();

	// find attack target
	GameEntity *foundEntity = nullptr;
	if (self->isEnemy())
	{
		for (const auto &entity : scene->getSelfSoldiers())
		{
			if (!entity->shouldClean() && area.containsPoint(entity->getPosition()))	
			{
				foundEntity = entity;
			}
		}	
	}
	else
	{
		for (const auto &entity : scene->getOppoSoldiers())
		{
			if (!entity->shouldClean() && area.containsPoint(entity->getPosition()))	
			{
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

	return false;
}

BT_REGISTER_PRECONDITION(CON_EnemyCanAttack)