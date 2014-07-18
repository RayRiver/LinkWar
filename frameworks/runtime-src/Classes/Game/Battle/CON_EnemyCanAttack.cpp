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

	// 寻找可攻击目标;
	GameObject *foundObject = nullptr;
	OBJECTS->callObjects([&](GameObject *object, GameObjectView *view) -> bool {
		if (!object->shouldClean() && object->group() != self->group()) // 对象不销毁，非同组;
		{
			if (area.containsPoint(object->getPosition()))
			{
				foundObject = object;
				return false;	// 寻找第一个可攻击目标;
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
