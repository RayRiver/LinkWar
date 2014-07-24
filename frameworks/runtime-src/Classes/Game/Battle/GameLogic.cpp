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
	// �����ƶ���ͼ�¼�;
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
			// ��������;
			auto self = OBJECTS->createObject(++m_objectIdIndex, 1, (int)GameObjectGroup::Group0);
			self->setPosition(point);
			self->setDirection(GameObjectDirection::Up);
			self->idle();

			// ��ʱ�����Է����������λ��;
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

	// �߼�֡����;
	objectManager->callObjects([&](int id, GameObject *object, GameObjectView *view) -> bool {
		if (object)
		{
			if (object->shouldClean())
			{
				// �������������б�;
				shouldCleanObjects.insert(std::make_pair(id, object));
			}
			else
			{
				// �����߼�����;
				object->onLogic();

				/*
				// �����ƶ�;
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
				// �����������ͼ�б�;
				shouldCleanObjectViews.insert(std::make_pair(id, view));
			}
		}

		return true;
	});

	// �������;
	for (auto it : shouldCleanObjects)
	{
		auto id = it.first;
		auto object = it.second;

		// �ͷŶ�����ռ�ĵ�ͼ����;
		auto &pos = object->getPosition();
		auto current_grid = mapManager->pos2grid(pos);
		mapManager->releaseGrid(current_grid.x, current_grid.y);

		// �Ƴ�����;
		objectManager->destroyObject(id);
	}
	shouldCleanObjects.clear();	

	// ������ͼ;
	for (auto it : shouldCleanObjectViews)
	{
		auto id = it.first;

		// �Ƴ�������ͼ;
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

			// debug find path count
			//DEBUGINFO_SETINT("find path count", DEBUGINFO_GETINT("find path count") + 1);

			// find path
			if (pathFinder.findPath())
			{
				target_grid = pathFinder.getPath().back();
				target = mapManager->grid2pos(target_grid);
				if (pos == target && pathFinder.getPath().size() > 0) // �ѵִ�·���Һ��滹��;
				{
					target_grid = pathFinder.getPath().back();
					pathFinder.getPath().pop_back();
					target = mapManager->grid2pos(target_grid);
				}
			}
			else
			{
				// Ѱ·ʧ�ܣ���ʱ����;
				target = pos;
				target_grid = current_grid;
			}
		}
	}

	// �ƶ���Ŀ��λ��;
	if (target != pos)
	{
		auto pt = target - pos;
		if (pt.x*pt.x + pt.y*pt.y > moveSpeed*moveSpeed)
		{
			pt.normalize();

			// ��������ٶȱ仯;
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
		
		// ���ó���;
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

		// ����ı�;
		if (direction != object->getDirection())
		{
			// ���Ŷ���;
			view->onMove();
		}

		// ����λ��;
		pt.x += pos.x;
		pt.y += pos.y;
		object->setPosition(pt);

		// ����map;
		auto current_grid2 = mapManager->pos2grid(pt);
		if (current_grid != current_grid2)
		{
			mapManager->releaseGrid(current_grid.x, current_grid.y);
			mapManager->retainGrid(current_grid2.x, current_grid2.y);
		}
	}
	*/
}