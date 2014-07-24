#include "CON_FoundPath.h"

#include "Helper/Display.h"

#include "BlackBoard.h"
#include "GameObjectMacros.h"
#include "GameObject.h"
#include "GameObjectManager.h"
#include "MapManager.h"

bool CON_FoundPath::onEvaluate(const BTInputParam& input) const
{
	const BlackBoard& inputData	= input.getRealData<BlackBoard>();

	auto object = inputData.self;


	auto mapManager = MapManager::getInstance();
	auto objectManager = GameObjectManager::getInstance();
	auto view = GAME_OBJECT_VIEW(object->id());

	auto pos = object->getPosition();
	auto target = object->getAttackTarget()->getPosition();
	auto &current_grid = mapManager->pos2grid(pos);
	auto target_grid = mapManager->pos2grid(target);
	auto &pathFinder = object->getPathFinder();
	auto moveSpeed = object->getMoveSpeed();

	// ��ȡ�����·��;
	if (current_grid != target_grid)
	{
		bool bNeedFindPath = false;
		if (pathFinder.getPath().size() > 0)
		{
			auto origin_target_grid = pathFinder.getPath().front();
			if (origin_target_grid != target_grid)
			{
				// Ŀ��λ���Ѿ����ϴ�Ѱ·�Ĳ�ͬ;
				bNeedFindPath = true;
			}
			else
			{
				// �Ѿ�����õ�·����Ч;
				target_grid = pathFinder.getPath().back();
				target = mapManager->grid2pos(target_grid);
				if (pos == target && pathFinder.getPath().size() > 0) // �ѵִ�·���Һ��滹��;
				{
					target_grid = object->getPathFinder().getPath().back();
					pathFinder.getPath().pop_back();
					target = mapManager->grid2pos(target_grid);
				}
			}
		}
		else
		{
			bNeedFindPath = true;
		}

		// Ŀ������ϰ�����Ҫ����Ѱ·;
		if (!bNeedFindPath)
		{
			int refCount = mapManager->getGridRef(target_grid.x, target_grid.y);
			if (current_grid == target_grid) --refCount;
			if (mapManager->pos2grid(object->getAttackTarget()->getPosition()) == target_grid) --refCount;
			if (refCount > 0)
			{
				pathFinder.clearPath();
				bNeedFindPath = true;
			}
		}

		if (bNeedFindPath)
		{
			// ����Ѱ·;
			pathFinder.setOrigin(current_grid);
			pathFinder.setDestination(target_grid);

			return false;
		}
		else
		{
			return true;
		}

	}
	else
	{
		return true;
	}

}

BT_REGISTER_PRECONDITION(CON_FoundPath)
