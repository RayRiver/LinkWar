#include "MapView.h"

#include "Helper/Display.h"

#include "MapManager.h"
#include "GameLogic.h"

USING_NS_CC;

const int RECT_POINTS = 4;
const float DRAG_MIN_DISTANCE2 = 20.0f;

MapView * MapView::create()
{
	MapView *ret = new MapView;
	if (ret && ret->init())
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

MapView::MapView()
	: m_draggingTouch(nullptr)
	, m_draggingMoveDistance2(0.0f)
{

}

MapView::~MapView()
{

}

bool MapView::init()
{
	if (!Layer::init())
	{
		return false;
	}

	m_mapW = MAP->mapW();
	m_mapH = MAP->mapH();
	m_gridW = MAP->gridW();
	m_gridH = MAP->gridH();
	m_displayW = MAP->displayW();
	m_displayH = MAP->displayH();

	createBg();
	createGrid();
	createLauncherArea();
	createBattleFieldArea();
	createTerrain();

	// ×¢²áµã»÷ÊÂ¼þ;
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [=](const std::vector<Touch*> &touches, Event *event) {
		CCASSERT(!m_draggingTouch, "");
		m_draggingTouch = touches[0];
	};
	listener->onTouchesMoved = [=](const std::vector<Touch*> &touches, Event *event) {
		CCASSERT(m_draggingTouch, "");
		for (const auto &touch : touches)
		{
			if (m_draggingTouch == touch)
			{
				auto lastPoint = (touch->getPreviousLocation());
				auto point = (touch->getLocation());

				m_draggingMoveDistance2 += (point - lastPoint).lengthSquared();
				if (m_draggingMoveDistance2 > DRAG_MIN_DISTANCE2)
				{
					GameLogic::getInstance()->handleDragMap(point - lastPoint);
				}

			}
		}
	};
	listener->onTouchesEnded = [=](const std::vector<Touch*> &touches, Event *event) {
		CCASSERT(m_draggingTouch, "");
		for (const auto &touch : touches)
		{
			auto lastPoint = (touch->getPreviousLocation());
			auto point = (touch->getLocation());

			m_draggingMoveDistance2 += (point - lastPoint).lengthSquared();
			if (m_draggingMoveDistance2 < DRAG_MIN_DISTANCE2)
			{
				GameLogic::getInstance()->handleTouch(point);
			}

			if ( m_draggingTouch == touch )
			{
				m_draggingTouch = nullptr;
				m_draggingMoveDistance2 = 0.0f;
			}
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void MapView::createBg()
{
	auto layer = LayerColor::create(Color4B(50, 50, 50, 255));
	this->addChild(layer, MapManager::LAYER_BG);
}

void MapView::createGrid()
{
	auto grid = DrawNode::create();
	this->addChild(grid, MapManager::LAYER_GRID);

	auto top = DisplayHelper::getInstance()->top();
	auto bottom = DisplayHelper::getInstance()->bottom();
	auto left = DisplayHelper::getInstance()->left();
	auto right = DisplayHelper::getInstance()->right();

	Fixed i;
	for (i=0; i<m_mapW; ++i)
	{
		grid->drawSegment(Vec2(i*m_gridW, top), Vec2(i*m_gridW, bottom), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
	for (i=0; i<m_mapH; ++i)
	{
		grid->drawSegment(Vec2(left, i*m_gridH), Vec2(right, i*m_gridH), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
}

void MapView::createLauncherArea()
{
	const auto &selfLauncherArea = MAP->getSelfLauncherArea().toRect();
	const auto &oppoLauncherArea = MAP->getOppoLauncherArea().toRect();

	Vec2 selfVerts[RECT_POINTS], oppoVerts[RECT_POINTS];
	DisplayHelper::getInstance()->rect2points(selfLauncherArea, selfVerts);
	DisplayHelper::getInstance()->rect2points(oppoLauncherArea, oppoVerts);

	auto draw = DrawNode::create();
	this->addChild(draw, MapManager::LAYER_LAUNCHER_AREA);

	draw->drawPolygon(selfVerts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 0.2f), 1.0f, Color4F(0.0f, 1.0f, 0.0f, 0.4f));
	draw->drawPolygon(oppoVerts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 0.2f), 1.0f, Color4F(1.0f, 0.0f, 0.0f, 0.4f));
}

void MapView::createBattleFieldArea()
{
	const auto &battleFieldArea = MAP->getBattleFieldArea().toRect();;

	auto draw = DrawNode::create();
	this->addChild(draw, MapManager::LAYER_BATTLE_FIELD_AREA);

	Vec2 verts[RECT_POINTS];
	DisplayHelper::getInstance()->rect2points(battleFieldArea, verts);
	draw->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 1.0f, 1.0f, 0.2f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.4f));
}

void MapView::createTerrain()
{
	auto draw = DrawNode::create();
	this->addChild(draw, MapManager::LAYER_TERRAIN);

	// draw
	Fixed x, y;
	for (y=0; y<m_mapH; ++y)
	{
		for (x=0; x<m_mapW; ++x)
		{
			auto key = MAP->grid2key(x, y);
			if (MAP->data(key).type != MapGrid::Type::None)
			{
				auto &pos_x = x * m_gridW;
				auto &pos_y = y * m_gridH;
				Vec2 verts[] = { Vec2(pos_x, pos_y), Vec2(pos_x, pos_y+m_gridH), Vec2(pos_x+m_gridW, pos_y+m_gridH), Vec2(pos_x+m_gridW, pos_y) };
				draw->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 1.0f, 1.0f, 0.8f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}
}

