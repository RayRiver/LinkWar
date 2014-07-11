#ifndef AStar_h__
#define AStar_h__

#include <vector>
#include <set>
#include <map>

#include "cocos2d.h"

class AStar
{
public:
	struct Point
	{
		int x, y;
		Point(int _x=-1, int _y=-1) : x(_x), y(_y) {}
		inline bool operator==(const Point &other)
		{
			return x == other.x && y == other.y;
		}
		inline bool operator!=(const Point &other)
		{
			return x != other.x || y != other.y;
		}
	};

public:
	AStar();
	~AStar();

	void loadMap(unsigned char *map, unsigned int w, unsigned int h);
	void setOrigin(const AStar::Point &point);
	void setDestination(const AStar::Point &point);

	bool findPath();
	inline std::vector<AStar::Point> &getPath() { return m_path; }
	inline void clearPath() { m_path.clear(); }

private:
	// G: 从起始状态到当前状态的实际代价（准确）
	// H: 从当前状态到目标状态的估计代价（曼哈顿法）
	// F: 从起始状态到目标状态的预计代价（G+H）
	struct Node
	{
		Node() : F(0.0f), G(0.0f), H(0.0f), parent(nullptr) { point.x = -1; point.y = -1; }
		AStar::Point point;
		float F, G, H;
		AStar::Node *parent;
	};

	bool _checkChild(int x, int y, AStar::Node *parent);
	void _clearNodes();
	//inline int _vec2key(const cocos2d::Vec2 &vec) { return vec.y * m_mapW + vec.x; }
	inline int _point2key(const AStar::Point &point) { return point.y * m_mapW + point.x; }
	inline int _xy2key(int x, int y) { return y * m_mapW + x; }
	/*
	inline cocos2d::Vec2 _key2vec(int key) 
	{ 
		cocos2d::Vec2 vec;
		vec.x = key % m_mapW;
		vec.y = key / m_mapW;
		return vec; 
	}
	*/
	inline AStar::Point _key2point(int key) 
	{ 
		AStar::Point point;
		point.x = key % m_mapW;
		point.y = key / m_mapW;
		return point; 
	}
	
private:
	unsigned char *m_map;
	unsigned int m_mapW, m_mapH;
	int m_orgX, m_orgY, m_desX, m_desY;
	std::vector<Node *> m_openList;
	std::map<int, Node *> m_openNodes;
	std::set<int> m_closeNodes;

	std::vector<AStar::Point> m_path;
};

#endif // AStar_h__
