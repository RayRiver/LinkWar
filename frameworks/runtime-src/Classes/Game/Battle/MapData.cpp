#include "MapData.h"

#include "cocos2d.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "editor-support/cocostudio/DictionaryHelper.h"

#include "Helper/Display.h"

USING_NS_CC;
using namespace cocostudio;

MapData::MapData()
	: m_mapData(nullptr)
	, m_tiledMap(nullptr)
{
	
}

MapData::~MapData()
{
	if (m_mapData)
	{
		delete[] m_mapData;
		m_mapData = nullptr;
	}
}

bool MapData::init( const char *config )
{
	m_displayW = DisplayHelper::getInstance()->width();
	m_displayH = DisplayHelper::getInstance()->height();

	m_tiledMap = TMXTiledMap::create(config);
	auto collisionLayer = m_tiledMap->getLayer("Collision");
	auto maskLayer = m_tiledMap->getLayer("Mask");
	collisionLayer->setVisible(false);
	maskLayer->setVisible(false);

	auto tileset = maskLayer->getTileSet();

	// 解析地图宽高;
	const auto &size = m_tiledMap->getMapSize();
	m_mapW = size.width;
	m_mapH = size.height;

	// 解析格子宽高;
	const auto &tileSize = m_tiledMap->getTileSize();
	m_gridW = tileSize.width;
	m_gridH = tileSize.height;

	// 创建地图数据;
	auto grids = (int)m_mapW * (int)m_mapH;
	m_mapData = new MapGridData[grids];
	if (!m_mapData)
	{
		return false;
	}


	// 解析地图数据;
	Fixed x, y;
	for (y=0; y<m_mapH; ++y)
	{
		for (x=0; x<m_mapW; ++x)
		{
			int index = y * m_mapW + x;
			auto &gridData = m_mapData[index];
			gridData.grid.x = x;
			gridData.grid.y = y;

			// 地形信息;
			auto gid = grid2gid(collisionLayer, x, y);
			auto properties = m_tiledMap->getPropertiesForGID(gid);
			if (properties.getType() == Value::Type::MAP)
			{
				auto dict = properties.asValueMap();
				auto it = dict.find("type");
				if (it == dict.end())
				{
					gridData.type = MapGrid::Type::None;
				}
				else
				{
					auto type = it->second.asInt();
					gridData.type = (MapGrid::Type)type;
				}
			}
			else
			{
				gridData.type = MapGrid::Type::None;
			}

			// 出兵点;
			auto mask_gid = grid2gid(maskLayer, x, y);
			auto mask_properties = m_tiledMap->getPropertiesForGID(mask_gid);
			if (mask_properties.getType() == Value::Type::MAP)
			{
				auto dict = mask_properties.asValueMap();
				auto it = dict.find("group");
				if (it == dict.end())
				{
					gridData.group = MapGrid::Group::None;
				}
				else
				{
					auto group = it->second.asInt();
					gridData.group = (MapGrid::Group)group;
				}
			}
			else
			{
				gridData.group = MapGrid::Group::None;
			}


			// 出兵点列表;
			if (gridData.group == MapGrid::Group::Group0)
			{
				m_selfLauncherGrids.push_back(gridData.grid);
			}
			else if (gridData.group == MapGrid::Group::Group1)
			{
				m_oppoLauncherGrids.push_back(gridData.grid);
			}
		}
	}

	return true;
}

unsigned int MapData::grid2gid( cocos2d::TMXLayer *layer, const MapGrid &grid )
{
	auto tiled_grid = grid;
	tiled_grid.y = m_mapH - Fixed::ONE - tiled_grid.y;
	return layer->getTileGIDAt(tiled_grid.toPoint());
}

unsigned int MapData::grid2gid( cocos2d::TMXLayer *layer, const Fixed &x, const Fixed &y )
{
	auto tiled_grid = MapGrid(x, y);
	tiled_grid.y = m_mapH - Fixed::ONE - tiled_grid.y;
	return layer->getTileGIDAt(tiled_grid.toPoint());
}

/*
{
	// 固定大小，地图可移动;
	m_gridW = 32;
	m_gridH = 32;

	m_displayW = DisplayHelper::getInstance()->width();
	m_displayH = DisplayHelper::getInstance()->height();

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(config);
	std::string fileContent = FileUtils::getInstance()->getStringFromFile(fullPath);

	auto doc = rapidjson::Document();
	doc.Parse<0>(fileContent.c_str());
	if (doc.HasParseError()) {
		CCLOG("GetParseError %s\n", doc.GetParseError());
		return false;
	}

	const rapidjson::Value &dict = doc;

	// 解析地图宽高信息;
	auto w = DICTOOL->getIntValue_json(dict, "w", 1);
	auto h = DICTOOL->getIntValue_json(dict, "h", 1);
	m_mapW = w;
	m_mapH = h;

	// 创建地图数据;
	auto grids = (int)m_mapW * (int)m_mapH;
	m_mapData = new MapGridData[grids];
	if (!m_mapData)
	{
		return false;
	}

	// 解析地图数据;
	auto count = DICTOOL->getArrayCount_json(dict, "data");
	Fixed x, y;
	for (y=0; y<m_mapH; ++y)
	{
		for (x=0; x<m_mapW; ++x)
		{
			int index = y * m_mapW + x;
			auto n = DICTOOL->getIntValueFromArray_json(dict, "data", index);
			auto &gridData = m_mapData[index];
			gridData.grid.x = x;
			gridData.grid.y = y;
			gridData.type = (MapGrid::Type)n;	

			// 出兵点;
			if (gridData.type == MapGrid::Type::Group0)
			{
				m_selfLauncherGrids.push_back(gridData.grid);
			}
			else if (gridData.type == MapGrid::Type::Group1)
			{
				m_oppoLauncherGrids.push_back(gridData.grid);
			}
		}
	}

	return true;
}
*/

