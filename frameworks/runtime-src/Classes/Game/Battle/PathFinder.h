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
	void setOrigin(const MapManager::Grid &grid);
	void setDestination(const MapManager::Grid &grid);

	bool findPath();
	inline std::vector<MapManager::Grid> &getPath() { return m_path; }
	inline void clearPath() { m_path.clear(); }

private:
	// G: ����ʼ״̬����ǰ״̬��ʵ�ʴ��ۣ�׼ȷ��
	// H: �ӵ�ǰ״̬��Ŀ��״̬�Ĺ��ƴ��ۣ������ٷ���
	// F: ����ʼ״̬��Ŀ��״̬��Ԥ�ƴ��ۣ�G+H��
	struct Node
	{
		Node() : F(0.0f), G(0.0f), H(0.0f), parent(nullptr) { point.x = -1; point.y = -1; }
		MapManager::Grid point;
		float F, G, H;
		PathFinder::Node *parent;
	};

	bool _checkChild(int x, int y, PathFinder::Node *parent);
	void _clearNodes();
	
private:
	MapManager *m_map;
	int m_orgX, m_orgY, m_desX, m_desY;

	std::vector<Node *> m_openList;
	std::map<int, Node *> m_openNodes;
	std::set<int> m_closeNodes;

	std::vector<MapManager::Grid> m_path;
};

#endif // PathFinder_h__
