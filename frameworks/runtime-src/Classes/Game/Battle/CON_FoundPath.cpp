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

	// 获取最近的路点;
	if (current_grid != target_grid)
	{
		bool bNeedFindPath = false;
		if (pathFinder.getPath().size() > 0)
		{
			auto origin_target_grid = pathFinder.getPath().front();
			if (origin_target_grid != target_grid)
			{
				// 目标位置已经和上次寻路的不同;
				bNeedFindPath = true;
			}
			else
			{
				// 已经计算好的路径有效;
				target_grid = pathFinder.getPath().back();
				target = mapManager->grid2pos(target_grid);
				if (pos == target && pathFinder.getPath().size() > 0) // 已抵达路点且后面还有;
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

		// 目标点有障碍，需要重新寻路;
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
			// 重新寻路;
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
