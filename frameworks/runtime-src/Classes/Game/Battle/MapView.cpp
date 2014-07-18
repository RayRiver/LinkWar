#include "MapView.h"

#include "Helper/Display.h"

#include "MapManager.h"

USING_NS_CC;

const int RECT_POINTS = 4;

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

	Fixed i;
	for (i=0; i<m_mapW; ++i)
	{
		grid->drawSegment(Vec2(i*m_gridW, display.top()), Vec2(i*m_gridW, display.bottom()), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
	for (i=0; i<m_mapH; ++i)
	{
		grid->drawSegment(Vec2(display.left(), i*m_gridH), Vec2(display.right(), i*m_gridH), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
}

void MapView::createLauncherArea()
{
	const auto &selfLauncherArea = MAP->getSelfLauncherArea().toRect();
	const auto &oppoLauncherArea = MAP->getOppoLauncherArea().toRect();

	Vec2 selfVerts[RECT_POINTS], oppoVerts[RECT_POINTS];
	display.rect2points(selfLauncherArea, selfVerts);
	display.rect2points(oppoLauncherArea, oppoVerts);

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
	display.rect2points(battleFieldArea, verts);
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
