#include "CON_FoundEnemy.h"

#include "Helper/Display.h"

#include "BlackBoard.h"
#include "GameObjectMacros.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "MapManager.h"

bool CON_FoundEnemy::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto self = inputData.self;

	/*
	// �Ѿ��й���������,�Ҹù���������Թ�����;
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
	Fixed distance2 = Fixed::ZERO;

	// Ѱ������Ĺ���Ŀ��;
	GameObject *foundObject = nullptr;
	OBJECTS->callObjects([&](int id, GameObject *object, GameObjectView *view) -> bool {
		if (object && !object->shouldClean() && object->group() != self->group()) // �������٣���ͬ��;
		{
			auto &d2 = (object->getPosition() - pos).lengthSquared();
			if (!foundObject || d2 < distance2)
			{
				distance2 = d2;
				foundObject = object;
			}
		}
		return true;
	});

	if (foundObject)
	{
		self->setAttackTarget(foundObject);
		return true;
	}
	else
	{
		return false;
	}
}

BT_REGISTER_PRECONDITION(CON_FoundEnemy)
