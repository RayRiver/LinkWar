#ifndef MapManager_h__
#define MapManager_h__

#include "cocos2d.h"

#include "MapStructs.h"

class GameObject;
class LogicFrame;
class MapData;
class MapView;

class MapManager : public cocos2d::Node
{
public:
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

public:
	static MapManager *getInstance();
	static void destroyInstance();
	~MapManager();

	bool loadData(const char *config);
	void reset();

	void handleLogicFrame(LogicFrame *frame);

	GameObject *createObject(const char *config, int id, bool isEnemy, const MapPoint &pos);
	void moveObject(GameObject *object);

	MapView *getView() { return m_view; }

	// 地图信息;
	const Fixed &mapW();
	const Fixed &mapH();
	const Fixed &gridW();
	const Fixed &gridH();
	const Fixed &displayW();
	const Fixed &displayH();
	const MapGridData &data(int key);

	const MapRect &getSelfLauncherArea();
	const MapRect &getOppoLauncherArea();

	// 地图格子引用;
	void retainGrid(const Fixed &x, const Fixed &y);
	void releaseGrid(const Fixed &x, const Fixed &y);
	int getGridRef(const Fixed &x, const Fixed &y);
	MapGrid::Type getGridType(const Fixed &x, const Fixed &y);
	bool isGridBarrier(const Fixed &x, const Fixed &y);

	// 辅助方法;
	MapGrid pos2grid(const MapPoint &pos);
	MapPoint grid2pos(const MapGrid &grid);
	int grid2key(const MapGrid &grid);
	int grid2key(const Fixed &x, const Fixed &y);
	void rect2points(const MapRect &r, MapPoint *points);

private:
	MapManager();
	static MapManager *s_instance;

	MapData *m_data;
	MapView *m_view;

	std::map<int, GameObject *> m_selfObjectMap;
	std::map<int, GameObject *> m_oppoObjectMap;

};

#define MAP MapManager::getInstance()

#endif // MapManager_h__
