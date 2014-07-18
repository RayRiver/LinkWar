#include "MapManager.h"

#include "Helper/Display.h"
#include "Helper/DebugInfo.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

#include "Base/LuaValueList.h"
#include "Base/NetManager.h"
#include "Base/NetSocket.h"

#include "MessageDef.h"
#include "GameEntity.h"
#include "LogicFrame.h"
#include "UserData.h"

USING_NS_CC;
using namespace cocostudio;

int g_id = 0;

enum
{
	LAYER_BG = -5,
	LAYER_GRID = -4,
	LAYER_TERRAIN = -3,
	LAYER_LAUNCHER_AREA = -2,
	LAYER_BATTLE_FIELD_AREA = -2,
	LAYER_MAP_DEBUGINFO = -1,

	LAYER_MAP_NODE_BEGIN = 0,
	// ...
	LAYER_MAP_NODE_END = 9999,

	LAYER_DEBUGINFO = 10000,
};

const int GRID_SIZE_W = 40;
const int GRID_SIZE_H = 40;
const int RECT_POINTS = 4;

MapManager::MapManager()
	: m_map(nullptr)
{

}

MapManager::~MapManager()
{
	if (m_map)
	{
		delete[] m_map;
		m_map = nullptr;
	}
}

MapManager * MapManager::create(const char *config)
{
	MapManager *ret = new MapManager;
	if (ret && ret->init(config))
	{
		ret->autorelease();
	}
	else
	{
		delete ret;
		ret = nullptr;
	}
	return ret;
}

bool MapManager::init(const char *config)
{
	if (!Layer::init())
	{
		return false;
	}

	createMap(config);	
	createBg();
	createGrid();
	createLauncherArea();
	createBattleFieldArea();
	createTerrain();

	DEBUGINFO_INIT(this);

	LogicFrameManager::getInstance()->init();

	LuaValueList args;	
	GetNetImp()->writePacket(NET_CS_READY, args);

	// touch event
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [=](const std::vector<Touch*> &touches, Event *event) {
		for (const auto &touch : touches)
		{
			const Vec2 &point = touch->getLocation();

			/*
			if (this->m_selfLauncherArea.containsPoint(point))
			{
				// create entity
				this->createEntity(++g_id, false, point);

				// TODO: create opposite entity at random position
				Vec2 p;
				p.x = rand() % (int)(m_oppoLauncherArea.size.width) + (int)(m_oppoLauncherArea.origin.x);
				p.y = rand() % (int)(m_oppoLauncherArea.size.height) + (int)(m_oppoLauncherArea.origin.y);
				this->createEntity(++g_id, true, p);
			}
			*/

			if (UserData::getInstance()->getUid()==0 && this->m_selfLauncherArea.containsPoint(point))
			{
				LuaValueList args;	
				args.pushFloat(point.x);
				args.pushFloat(point.y);
				GetNetImp()->writePacket(NET_CS_CREATE_ENTITY, args);
			}
			else if (UserData::getInstance()->getUid()==1 && this->m_oppoLauncherArea.containsPoint(point))
			{
				LuaValueList args;	
				args.pushFloat(point.x);
				args.pushFloat(point.y);
				GetNetImp()->writePacket(NET_CS_CREATE_ENTITY, args);
			}
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	// schedule update
	this->scheduleUpdate();

	return true;
}

void MapManager::update( float dt )
{
	while (true)
	{
		auto frame = LogicFrameManager::getInstance()->getFrame();
		if (!frame)
		{
			// no frame
			return;
		}

		// handle frame
		for (auto action : frame->actions)
		{
			this->handleFrame(action);
		}

		// debug logic frame index
		DEBUGINFO_SETINT("logic frame index", LogicFrameManager::getInstance()->getCurrentFrameIndex());


		std::map<int, GameEntity *> cleanSelfEntities;
		for (auto it : m_selfEntities)
		{
			auto id = it.first;
			auto entity = it.second;
			if (entity->shouldClean())
			{
				cleanSelfEntities.insert(std::make_pair(id, entity));
			}
			else
			{
				entity->onFrame(dt);
				if (entity->desireMove())
				{
					this->moveEntity(entity);
				}
			}
		}
		std::map<int, GameEntity *> cleanOppoEntities;
		for (auto it : m_oppoEntities)
		{
			auto id = it.first;
			auto entity = it.second;
			if (entity->shouldClean())
			{
				cleanOppoEntities.insert(std::make_pair(id, entity));
			}
			else
			{
				entity->onFrame(dt);
				if (entity->desireMove())
				{
					this->moveEntity(entity);
				}
			}
		}

		// clean children
		for (auto it : cleanSelfEntities)
		{
			auto id = it.first;
			auto entity = it.second;

			auto pos = entity->getPosition();
			auto current_grid = pos2grid(pos);
			releaseGrid(current_grid.x, current_grid.y);

			m_selfEntities.erase(id);
			this->removeChild(entity);
		}
		for (auto it : cleanOppoEntities)
		{
			auto id = it.first;
			auto entity = it.second;

			auto pos = entity->getPosition();
			auto current_grid = pos2grid(pos);
			releaseGrid(current_grid.x, current_grid.y);

			m_oppoEntities.erase(id);
			this->removeChild(entity);
		}

	}
}

void MapManager::createMap( const char *config )
{
	// TODO: 根据配置文件来算grid_size
	m_w = display.width() / GRID_SIZE_W;
	m_h = display.height() / GRID_SIZE_H;
	m_map = new unsigned char[m_w*m_h];
	memset(m_map, 0x00, m_w*m_h);


	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(config);
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	auto doc = rapidjson::Document();
	doc.Parse<0>(fileContent.c_str());
	if (doc.HasParseError()) {
		CCLOG("GetParseError %s\n", doc.GetParseError());
		return;
	}

	const rapidjson::Value &dict = doc;

	// parse properties
	auto w = DICTOOL->getIntValue_json(dict, "w", 1);
	auto h = DICTOOL->getIntValue_json(dict, "h", 1);

	auto count = DICTOOL->getArrayCount_json(dict, "data");
	for (int i=0; i<count; ++i)
	{
		auto n = DICTOOL->getIntValueFromArray_json(dict, "data", i);
		m_map[i] = (unsigned char)n;
	}

}

void MapManager::createBg()
{
	auto layer = LayerColor::create(Color4B(50, 50, 50, 255));
	this->addChild(layer, LAYER_BG);
}

void MapManager::createGrid()
{
	auto grid = DrawNode::create();
	this->addChild(grid, LAYER_GRID);

	for (int i=0; i<m_w; ++i)
	{
		grid->drawSegment(Vec2(i*GRID_SIZE_W, display.top()), Vec2(i*GRID_SIZE_W, display.bottom()), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
	for (int i=0; i<m_h; ++i)
	{
		grid->drawSegment(Vec2(display.left(), i*GRID_SIZE_H), Vec2(display.right(), i*GRID_SIZE_H), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
}

void MapManager::createLauncherArea()
{
	m_selfLauncherArea = Rect(GRID_SIZE_W, 0, GRID_SIZE_W*(m_w-2), GRID_SIZE_H*4);
	m_oppoLauncherArea = Rect(GRID_SIZE_W, display.height()-GRID_SIZE_H*4, GRID_SIZE_W*(m_w-2), GRID_SIZE_H*4);
	
	Vec2 selfVerts[RECT_POINTS], oppoVerts[RECT_POINTS];
	display.rect2points(m_selfLauncherArea, selfVerts);
	display.rect2points(m_oppoLauncherArea, oppoVerts);

	auto draw = DrawNode::create();
	this->addChild(draw, LAYER_LAUNCHER_AREA);

	draw->drawPolygon(selfVerts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 0.2f), 1.0f, Color4F(0.0f, 1.0f, 0.0f, 0.4f));
	draw->drawPolygon(oppoVerts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 0.2f), 1.0f, Color4F(1.0f, 0.0f, 0.0f, 0.4f));
}

void MapManager::createBattleFieldArea()
{
	auto x = GRID_SIZE_W;
	auto y = GRID_SIZE_H*4;
	auto w = GRID_SIZE_W*(m_w-2);
	auto h = GRID_SIZE_H*(m_h-8);
	m_battleFieldArea = Rect(x, y, w, h);

	auto draw = DrawNode::create();
	this->addChild(draw, LAYER_BATTLE_FIELD_AREA);

	Vec2 verts[RECT_POINTS];
	display.rect2points(m_battleFieldArea, verts);
	draw->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 1.0f, 1.0f, 0.2f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.4f));
}

void MapManager::createTerrain()
{
	auto draw = DrawNode::create();
	this->addChild(draw, LAYER_TERRAIN);

	// draw
	for (int y=0; y<m_h; ++y)
	{
		for (int x=0; x<m_w; ++x)
		{
			auto key = grid2key(x, y);
			if (m_map[key])
			{
				auto pos_x = x * GRID_SIZE_W;
				auto pos_y = y * GRID_SIZE_H;
				Vec2 verts[] = { Vec2(pos_x, pos_y), Vec2(pos_x, pos_y+GRID_SIZE_H), Vec2(pos_x+GRID_SIZE_W, pos_y+GRID_SIZE_H), Vec2(pos_x+GRID_SIZE_W, pos_y) };
				draw->drawPolygon(verts, 4, Color4F(1.0f, 1.0f, 1.0f, 0.8f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}
}

GameEntity * MapManager::createEntity( int id, bool isEnemy, const cocos2d::Vec2 &pos )
{
	auto entity = GameEntity::create("config/entity_soldier.json", id, this, isEnemy);

	entity->setPosition(pos);
	entity->updateZOrder();
	this->addChild(entity);

	if (isEnemy)
	{
		m_oppoEntities.insert(std::make_pair(id, entity));
	}
	else
	{
		m_selfEntities.insert(std::make_pair(id, entity));
	}

	return entity;
}

void MapManager::retainGrid( int x, int y )
{
	if (m_map[y*m_w+x] == GridType::Barrier)
	{
		return;
	}
	m_map[y*m_w+x] += 1;
}

void MapManager::releaseGrid( int x, int y )
{
	if (m_map[y*m_w+x] == GridType::Barrier)
	{
		return;
	}
	else if (m_map[y*m_w+x] > 0)
	{
		m_map[y*m_w+x] -= 1;	
	}
	else
	{
		log("releaseGrid error, ref is 0: %d %d", x, y);	
	}
}

void MapManager::moveEntity( GameEntity *entity )
{
	auto pos = entity->getPosition();
	auto target = entity->getDesiredPosition();
	auto current_grid = pos2grid(pos);
	auto target_grid = pos2grid(target);
	auto &pathFinder = entity->getPathFinder();
	auto moveSpeed = entity->getMoveSpeed();

	// 获取最近的路点
	if (current_grid != target_grid)
	{
		bool bNeedFindPath = false;
		if (pathFinder.getPath().size() > 0)
		{
			auto origin_target_grid = pathFinder.getPath().front();
			if (origin_target_grid != target_grid)
			{
				// 目标位置已经和上次寻路的不同
				bNeedFindPath = true;
			}
			else
			{
				// 已经计算好的路径有效
				target_grid = pathFinder.getPath().back();
				target = grid2pos(target_grid);
				if (pos == target && pathFinder.getPath().size() > 0) // 已抵达路点且后面还有
				{
					target_grid = entity->getPathFinder().getPath().back();
					pathFinder.getPath().pop_back();
					target = grid2pos(target_grid);
				}
			}
		}
		else
		{
			bNeedFindPath = true;
		}

		// 目标点有障碍，需要重新寻路
		if (!bNeedFindPath)
		{
			int refCount = this->getGridRef(target_grid.x, target_grid.y);
			if (current_grid == target_grid) --refCount;
			if (refCount > 0)
			{
				pathFinder.clearPath();
				bNeedFindPath = true;
			}
		}

		if (bNeedFindPath)
		{
			// 重新寻路
			pathFinder.setOrigin(current_grid);
			pathFinder.setDestination(target_grid);

			// debug find path count
			DEBUGINFO_SETINT("find path count", DEBUGINFO_GETINT("find path count") + 1);

			// find path
			if (pathFinder.findPath())
			{
				target_grid = pathFinder.getPath().back();
				target = grid2pos(target_grid);
				if (pos == target && pathFinder.getPath().size() > 0) // 已抵达路点且后面还有
				{
					target_grid = pathFinder.getPath().back();
					pathFinder.getPath().pop_back();
					target = grid2pos(target_grid);
				}
			}
			else
			{
				// 寻路失败，暂时不动
				target = pos;
				target_grid = current_grid;
			}
		}
	}

	// 移动到目标位置
	if (target != pos)
	{
		auto pt = target - pos;
		if (pt.lengthSquared() > moveSpeed*moveSpeed)
		{
			pt.normalize();
			pt *= moveSpeed;
		}
		pt.x += pos.x;
		pt.y += pos.y;
		entity->setPosition(pt);
		entity->updateZOrder();	

		// 更新map
		auto current_grid2 = pos2grid(pt);
		if (current_grid != current_grid2)
		{
			this->releaseGrid(current_grid.x, current_grid.y);
			this->retainGrid(current_grid2.x, current_grid2.y);
		}
	}
}

void MapManager::handleFrame( LogicFrameAction *action )
{
	if (!action->uid)
	{
		if (action->type == LogicFrameAction::Type::CreateEntity)
		{
			Vec2 pos(action->x, action->y);
			this->createEntity(action->id, false, pos);
		}
	}
	else
	{
		if (action->type == LogicFrameAction::Type::CreateEntity)
		{
			this->createEntity(action->id, true, Vec2(action->x, action->y));
		}
	}
}

