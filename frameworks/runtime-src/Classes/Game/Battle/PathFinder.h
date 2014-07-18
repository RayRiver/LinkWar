#ifndef PathFinder_h__
#define PathFinder_h__

#include <vector>
#include <set>
#include <map>

#include "MapManager.h"

class PathFinder
{
public:
	PathFinder();
	~PathFinder();

	void init(MapManager *map);
	void setOrigin(const MapGrid &grid);
	void setDestination(const MapGrid &grid);

	bool findPath();
	inline std::vector<MapGrid> &getPath() { return m_path; }
	inline void clearPath() { m_path.clear(); }

private:
	// G: 从起始状态到当前状态的实际代价（准确）;
	// H: 从当前状态到目标状态的估计代价（曼哈顿法）;
	// F: 从起始状态到目标状态的预计代价（G+H）;
	struct Node
	{
		Node() : F(0.0f), G(0.0f), H(0.0f), parent(nullptr) { point.x = -1; point.y = -1; }
		MapGrid point;
		Fixed F, G, H;
		PathFinder::Node *parent;
	};

	bool _checkChild(const Fixed &x, const Fixed &y, PathFinder::Node *parent);
	void _clearNodes();
	
private:
	MapManager *m_map;
	Fixed m_mapW, m_mapH;
	Fixed m_orgX, m_orgY, m_desX, m_desY;

	int m_gridMaxContainCount;

	std::vector<Node *> m_openList;
	std::map<int, Node *> m_openNodes;
	std::set<int> m_closeNodes;

	std::vector<MapGrid> m_path;
};

#endif // PathFinder_h__
