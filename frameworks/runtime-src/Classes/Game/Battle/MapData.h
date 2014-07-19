#ifndef MapData_h__
#define MapData_h__

#include <map>

#include "Base/Fixed.h"

#include "MapStructs.h"

class GameObjectData;
class MapData
{
	friend class MapManager;

public:
	MapData();
	~MapData();

private:
	bool init(const char *config);

private:
	MapGridData *m_mapData;
	Fixed m_mapW, m_mapH;
	Fixed m_gridW, m_gridH;
	Fixed m_displayW, m_displayH;

	std::vector<MapGrid> m_selfLauncherGrids;
	std::vector<MapGrid> m_oppoLauncherGrids;

};

#endif // MapData_h__
