#include "MapManager.h"

#include "MapData.h"
#include "MapView.h"

#include "GameObject.h"
#include "GameObjectManager.h"

USING_NS_CC;

MapManager *MapManager::s_instance = nullptr;

MapManager * MapManager::getInstance()
{
	if (!s_instance)
	{
		s_instance = new MapManager;
		s_instance->init();

		// 不再交由自动释放池管理;
	}
	return s_instance;
}

void MapManager::destroyInstance()
{
	delete s_instance;
	s_instance = nullptr;
}

MapManager::MapManager()
	: m_data(nullptr)
	, m_view(nullptr)
{

}

MapManager::~MapManager()
{
	reset();
}

bool MapManager::loadData(const char *config)
{
	reset();

	// 创建地图数据;
	m_data = new MapData;
	if (!m_data || !m_data->init(config))
	{
		return false;
	}

	// 创建地图视图;
	m_view = MapView::create();
	this->addChild(m_view);

	return true;
}

void MapManager::reset()
{
	if (m_data)
	{
		delete m_data;
	}
	m_data = nullptr;

	m_view = nullptr;
}

const Fixed & MapManager::mapW()
{
	return m_data->m_mapW;
}

const Fixed & MapManager::mapH()
{
	return m_data->m_mapH;
}

const Fixed & MapManager::gridW()
{
	return m_data->m_gridW;
}

const Fixed & MapManager::gridH()
{
	return m_data->m_gridH;
}

const Fixed & MapManager::displayW()
{
	return m_data->m_displayW;
}

const Fixed & MapManager::displayH()
{
	return m_data->m_displayH;
}

const MapGridData & MapManager::data( int key )
{
	return m_data->m_mapData[key];
}

MapGrid MapManager::pos2grid(const MapPoint &pos) 
{
	MapGrid grid;
	grid.x = (int)pos.x / (int)m_data->m_gridW;
	grid.y = (int)pos.y / (int)m_data->m_gridH;
	return grid;
}

MapPoint MapManager::grid2pos(const MapGrid &grid) 
{
	MapPoint pos;
	pos.x = (grid.x+Fixed(0.5)) * m_data->m_gridW;
	pos.y = (grid.y+Fixed(0.5)) * m_data->m_gridH;
	return pos;
}

int MapManager::grid2key(const MapGrid &grid) 
{
	return grid.y*m_data->m_mapW + grid.x;
}

int MapManager::grid2key(const Fixed &x, const Fixed &y)
{
	return y*m_data->m_mapW + x;
}

void MapManager::retainGrid( const Fixed &x, const Fixed &y )
{
	int key = grid2key(x, y);
	if (m_data->m_mapData[key].type == MapGrid::Type::Barrier)
	{
		return;
	}
	m_data->m_mapData[key].refCount += 1;
}

void MapManager::releaseGrid( const Fixed &x, const Fixed &y )
{
	int key = grid2key(x, y);
	if (m_data->m_mapData[key].type == MapGrid::Type::Barrier)
	{
		return;
	}
	else if (m_data->m_mapData[key].refCount > 0)
	{
		m_data->m_mapData[key].refCount -= 1;	
	}
	else
	{
		log("releaseGrid error, ref is 0: %d %d", x, y);	
	}
}

int MapManager::getGridRef( const Fixed &x, const Fixed &y )
{
	int key = grid2key(x, y);
	return m_data->m_mapData[key].refCount;
}

MapGrid::Type MapManager::getGridType( const Fixed &x, const Fixed &y )
{
	int key = grid2key(x, y);
	return m_data->m_mapData[key].type;
}

bool MapManager::isGridBarrier( const Fixed &x, const Fixed &y )
{
	int key = grid2key(x, y);
	return MapGrid::Type::Barrier == m_data->m_mapData[key].type;
}

void MapManager::rect2points( const MapRect &r, MapPoint *points )
{
	points[0].x = r.x;
	points[0].y = r.y;
	points[1].x = r.x + r.w;
	points[1].y = r.y;
	points[2].x = r.x + r.w;
	points[2].y = r.y + r.h;
	points[3].x = r.x;
	points[3].y = r.y + r.h;
}

const std::vector<MapGrid> & MapManager::getSelfLauncherGrids()
{
	return m_data->m_selfLauncherGrids;
}

const std::vector<MapGrid> & MapManager::getOppoLauncherGrids()
{
	return m_data->m_oppoLauncherGrids;
}

