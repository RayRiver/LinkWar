#include "CON_FoundEnemy.h"

#include "Helper/Display.h"

#include "BlackBoard.h"
#include "MapManager.h"
#include "GameEntity.h"

bool CON_FoundEnemy::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto map = inputData.mapManager;
	auto self = inputData.self;

	/*
	// 已经有攻击对象了,且该攻击对象可以攻击到
	if (self->getState() == GameEntity::State::MoveToAttack && self->getAttackTarget())
	{
		auto attackTarget = self->getAttackTarget();
		if (scene->canTargetBeAttacked(attackTarget))
		{
			return true;
		}
	}
	*/

	auto pos = self->getPosition();
	float distance2 = 0.0f;

	// find attack target
	GameEntity *foundEntity = nullptr;
	if (self->isEnemy())
	{
		for (const auto &entity : map->getSelfEntities())
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
		for (const auto &entity : map->getOppoEntities())
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
