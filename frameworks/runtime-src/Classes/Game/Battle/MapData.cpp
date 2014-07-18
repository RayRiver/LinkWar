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
	m_displayW = display.width();
	m_displayH = display.height();

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
	m_gridW = m_displayW / m_mapW;
	m_gridH = m_displayH / m_mapH;

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
		}
	}

	// 战场区域;
	m_selfLauncherArea = MapRect(m_gridW, Fixed(0), m_gridW*(m_mapW-Fixed(2)), m_gridH*Fixed(4));
	m_oppoLauncherArea = MapRect(m_gridW, m_displayH-m_gridH*Fixed(4), m_gridW*(m_mapW-Fixed(2)), m_gridH*Fixed(4));
	m_battleFieldArea = MapRect(m_gridW, m_gridH*Fixed(4), m_gridW*(m_mapW-Fixed(2)), m_gridH*(m_mapH-Fixed(8)));

	return true;
}


