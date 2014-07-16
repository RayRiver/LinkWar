#ifndef MapManager_h__
#define MapManager_h__

#include <map>

#include "cocos2d.h"

extern const int GRID_SIZE_W;
extern const int GRID_SIZE_H;

class GameEntity;
struct LogicFrameAction;
class MapManager : public cocos2d::Layer
{
public:
	struct Grid
	{
		int x, y;
		Grid(int _x=-1, int _y=-1) : x(_x), y(_y) {}
		inline bool operator==(const Grid &other) {
			return x==other.x && y==other.y;
		}
		inline bool operator!=(const Grid &other) {
			return x!=other.x || y!=other.y;
		}
	};

	enum GridType
	{
		Barrier = 0xff,
	};

public:
	MapManager();
	~MapManager();

	static MapManager *create(const char *config);
	bool init(const char *config);
	void update(float dt);

	void updateEntity(GameEntity *entity, float dt);

	void createMap(const char *config);
	void createBg();
	void createGrid();
	void createLauncherArea();
	void createBattleFieldArea();
	void createTerrain();

	GameEntity *createEntity(int id, bool isEnemy, const cocos2d::Vec2 &pos);

	void moveEntity(GameEntity *entity);

	void handleFrame(LogicFrameAction *action);

	inline const unsigned char *getMap() { return m_map; }
	inline int getMapW() { return m_w; }
	inline int getMapH() { return m_h; }

	void retainGrid(int x, int y);
	void releaseGrid(int x, int y);
	inline int getGridRef(int x, int y) { return m_map[y*m_w+x]; }

	inline const cocos2d::Rect &getBattleFieldArea() { return m_battleFieldArea; }
	inline std::map<int, GameEntity *> &getSelfEntities() { return m_selfEntities; }
	inline std::map<int, GameEntity *> &getOppoEntities() { return m_oppoEntities; }

public:
	inline Grid pos2grid(const cocos2d::Vec2 &pos) {
		Grid grid;
		grid.x = (int)pos.x / GRID_SIZE_W;
		grid.y = (int)pos.y / GRID_SIZE_H;
		return grid;
	}
	inline cocos2d::Vec2 grid2pos(const Grid &grid) {
		cocos2d::Vec2 pos;
		pos.x = (grid.x+0.5) * GRID_SIZE_W;
		pos.y = (grid.y+0.5) * GRID_SIZE_H;
		return pos;
	}
	inline int grid2key(const Grid &grid) {
		return grid.y*m_w + grid.x;
	}
	inline int grid2key(int x, int y) {
		return y*m_w + x;
	}

private:
	unsigned char *m_map;
	int m_w, m_h;

	cocos2d::Rect m_selfLauncherArea;
	cocos2d::Rect m_oppoLauncherArea;
	cocos2d::Rect m_battleFieldArea;

	std::map<int, GameEntity *> m_selfEntities;
	std::map<int, GameEntity *> m_oppoEntities;
};

#endif // MapManager_h__
