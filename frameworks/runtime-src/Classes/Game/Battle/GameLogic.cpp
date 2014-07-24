#include "GameLogic.h"

#include <map>
using namespace std;

#include "LogicFrame.h"
#include "MapManager.h"
#include "MapView.h"
#include "GameObjectManager.h"
#include "GameObject.h"
#include "GameObjectView.h"

USING_NS_CC;

GameLogic * GameLogic::s_instance = nullptr;

GameLogic * GameLogic::getInstance()
{
	if (!s_instance)
	{
		s_instance = new GameLogic;
	}
	return s_instance;
}

GameLogic::GameLogic()
	: m_objectIdIndex(0)
{

}

GameLogic::~GameLogic()
{

}

void GameLogic::handleDragMap( const Vec2 &vec )
{
	// 处理移动地图事件;
	MAP->getView()->dragMap(vec);

}

void GameLogic::handleTouch( const Vec2 & pos )
{
	Vec2 realpos = pos - MAP->getView()->getPosition();

	const auto &point = MapPoint(realpos);
	const auto &touch_grid = MAP->pos2grid(point);

	const auto &selfGrids = MAP->getSelfLauncherGrids();
	const auto &oppoGrids = MAP->getOppoLauncherGrids();

	for (const auto &grid : selfGrids)
	{
		if (touch_grid == grid)
		{
			// 创建对象;
			auto self = OBJECTS->createObject(++m_objectIdIndex, 1, (int)GameObjectGroup::Group0);
			self->setPosition(point);
			self->setDirection(GameObjectDirection::Up);
			self->idle();

			// 临时创建对方对象，在随机位置;
			auto index = rand() % oppoGrids.size();
			auto p = MAP->grid2pos(oppoGrids[index]);
			auto oppo = OBJECTS->createObject(++m_objectIdIndex, 1, (int)GameObjectGroup::Group1);
			oppo->setPosition(p);
			oppo->setDirection(GameObjectDirection::Down);
			GAME_OBJECT_VIEW(oppo->id())->setColorMask(cocos2d::Color3B(255, 0, 0));
			oppo->idle();

			break;
		}
	}

}

void GameLogic::handleLogicFrame( LogicFrame *frame )
{
	auto mapManager = MapManager::getInstance();
	auto objectManager = GameObjectManager::getInstance();

	std::map<int, GameObject *> shouldCleanObjects;
	std::map<int, GameObjectView *> shouldCleanObjectViews;

	// 逻辑帧处理;
	objectManager->callObjects([&](int id, GameObject *object, GameObjectView *view) -> bool {
		if (object)
		{
			if (object->shouldClean())
			{
				// 加入待清理对象列表;
				shouldCleanObjects.insert(std::make_pair(id, object));
			}
			else
			{
				// 对象逻辑处理;
				object->onLogic();

				/*
				// 对象移动;
				if (object->desireMove())
				{
					this->moveObject(object);
				}
				*/
			}
		}

		if (view)
		{
			if (view->shouldClean())
			{
				// 加入待清理视图列表;
				shouldCleanObjectViews.insert(std::make_pair(id, view));
			}
		}

		return true;
	});

	// 清理对象;
	for (auto it : shouldCleanObjects)
	{
		auto id = it.first;
		auto object = it.second;

		// 释放对象所占的地图格子;
		auto &pos = object->getPosition();
		auto current_grid = mapManager->pos2grid(pos);
		mapManager->releaseGrid(current_grid.x, current_grid.y);

		// 移除对象;
		objectManager->destroyObject(id);
	}
	shouldCleanObjects.clear();	

	// 清理视图;
	for (auto it : shouldCleanObjectViews)
	{
		auto id = it.first;

		// 移除对象视图;
		objectManager->destroyObjectView(id);
	}
	shouldCleanObjectViews.clear();
}

void GameLogic::moveObject( GameObject *object )
{
	/*
	auto mapManager = MapManager::getInstance();
	auto objectManager = GameObjectManager::getInstance();
	auto view = GAME_OBJECT_VIEW(object->id());

	auto pos = object->getPosition();
	auto target = object->getDesiredPosition();
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

			// debug find path count
			//DEBUGINFO_SETINT("find path count", DEBUGINFO_GETINT("find path count") + 1);

			// find path
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
		}
	}

	// 移动到目标位置;
	if (target != pos)
	{
		auto pt = target - pos;
		if (pt.x*pt.x + pt.y*pt.y > moveSpeed*moveSpeed)
		{
			pt.normalize();

			// 特殊地形速度变化;
			if (MAP->getGridType(current_grid.x, current_grid.y) == MapGrid::Type::Mud)
			{
				moveSpeed *= Fixed(0.5);
			}
			else if (MAP->getGridType(current_grid.x, current_grid.y) == MapGrid::Type::Ice)
			{
				moveSpeed *= Fixed(2);
			}

			pt.x *= moveSpeed;
			pt.y *= moveSpeed;
		}
		
		// 设置朝向;
		auto direction = object->getDirection();
		if (pt.x.abs() > pt.y.abs())
		{
			if (pt.x > Fixed::ZERO)
			{
				object->setDirection(GameObjectDirection::Right);
			}
			else
			{
				object->setDirection(GameObjectDirection::Left);
			}
		}
		else
		{
			if (pt.y > Fixed::ZERO)
			{
				object->setDirection(GameObjectDirection::Up);
			}
			else
			{
				object->setDirection(GameObjectDirection::Down);
			}
		}

		// 朝向改变;
		if (direction != object->getDirection())
		{
			// 播放动画;
			view->onMove();
		}

		// 更新位置;
		pt.x += pos.x;
		pt.y += pos.y;
		object->setPosition(pt);

		// 更新map;
		auto current_grid2 = mapManager->pos2grid(pt);
		if (current_grid != current_grid2)
		{
			mapManager->releaseGrid(current_grid.x, current_grid.y);
			mapManager->retainGrid(current_grid2.x, current_grid2.y);
		}
	}
	*/
}