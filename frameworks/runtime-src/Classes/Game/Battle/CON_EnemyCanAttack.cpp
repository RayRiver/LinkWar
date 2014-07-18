#include "CON_EnemyCanAttack.h"

#include "BlackBoard.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"

bool CON_EnemyCanAttack::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto self = inputData.self;

	auto area = self->getAttackArea();
	area.x += self->getPositionX();
	area.y += self->getPositionY();

	// Ѱ�ҿɹ���Ŀ��;
	GameObject *foundObject = nullptr;
	OBJECTS->callObjects([&](GameObject *object, GameObjectView *view) -> bool {
		if (!object->shouldClean() && object->group() != self->group()) // �������٣���ͬ��;
		{
			if (area.containsPoint(object->getPosition()))
			{
				foundObject = object;
				return false;	// Ѱ�ҵ�һ���ɹ���Ŀ��;
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

	return false;
}

BT_REGISTER_PRECONDITION(CON_EnemyCanAttack)
