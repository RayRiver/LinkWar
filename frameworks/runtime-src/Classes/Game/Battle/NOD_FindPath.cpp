#include "NOD_FindPath.h"

#include "cocos2d.h"

#include "BlackBoard.h"
#include "GameObject.h"
#include "GameObjectManager.h"

USING_NS_CC;

void NOD_FindPath::onEnter( const BTInputParam &input )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();

	auto object = inputData.self;


	auto mapManager = MapManager::getInstance();
	auto objectManager = GameObjectManager::getInstance();
	auto view = GAME_OBJECT_VIEW(object->id());

	auto pos = object->getPosition();
	auto &current_grid = mapManager->pos2grid(pos);
	auto &pathFinder = object->getPathFinder();
	auto moveSpeed = object->getMoveSpeed();

	MapPoint target;
	MapGrid target_grid;

	if (pathFinder.findPath())
	{
		target_grid = pathFinder.getPath().back();
		target = mapManager->grid2pos(target_grid);
		if (pos == target && pathFinder.getPath().size() > 0) // 已抵达路点且后面还有;
		{
			target_grid = pathFinder.getPath().back();
			pathFinder.getPath().pop_back();
			target = mapManager->grid2pos(target_grid);
		}
	}
	else
	{
		// 寻路失败，暂时不动;
		target = pos;
		target_grid = current_grid;
	}

	object->setHasFrameMoveTarget(true);
	object->setFrameMoveTarget(target);
}

void NOD_FindPath::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus NOD_FindPath::onExecute( const BTInputParam& input, BTOutputParam& output )
{
	const BlackBoard &inputData = input.getRealData<BlackBoard>();
	BlackBoard &outputData = output.getRealData<BlackBoard>();

	return BTRunningStatus::Finish;
}

BT_REGISTER_ACTION_NODE(NOD_FindPath);
