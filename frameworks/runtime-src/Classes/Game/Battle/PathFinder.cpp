#include "PathFinder.h"

#include "Helper/Utils.h"

#include <math.h>

using namespace std;

const Fixed CORNER_COST = Fixed(1.414f);
const Fixed SIDE_COST = Fixed(1.0f);

// ����ѱȽ��㷨����Fֵ�Ƚϣ�С���ѣ�;
template <typename T>
class MinBinaryHeapCompare
{
public:
	bool operator() ( T a, T b ) const
	{
		return a->F>b->F;
	}
};

// �����ٷ�����Hֵ;
static Fixed _calcH(const Fixed &x1, const Fixed &y1, const Fixed &x2, const Fixed &y2)
{
	return ((x2-x1).abs()+(y2-y1).abs()) * SIDE_COST;
}

PathFinder::PathFinder()
	: m_map(nullptr)
	, m_orgX(-1)
	, m_orgY(-1)
	, m_desX(-1)
	, m_desY(-1)
{
	m_gridMaxContainCount = Utils::getGlobalSettingInt("map_grid_max_contain_objects_count", 1);
}

PathFinder::~PathFinder()
{
	_clearNodes();	
}

void PathFinder::init( MapManager *map )
{
	m_map = map;
	m_mapW = map->mapW();
	m_mapH = map->mapH();
}

void PathFinder::setOrigin( const MapGrid &grid )
{
	m_orgX = grid.x;
	m_orgY = grid.y;
}

void PathFinder::setDestination( const MapGrid &grid )
{
	m_desX = grid.x;
	m_desY = grid.y;
}

bool PathFinder::findPath()
{
	if (m_map==nullptr || 
		m_orgX<Fixed(0) || m_orgY<Fixed(0) || m_orgX>=m_mapW || m_orgY>=m_mapH ||
		m_desX<Fixed(0) || m_desY<Fixed(0) || m_desX>=m_mapW || m_desY>=m_mapH)
	{ 
		CCASSERT(false, ""); 
		return false; 
	}

	// ��ʼ�ڵ���뿪���б�;
	Node *startNode = new Node;
	startNode->point.x = m_orgX;
	startNode->point.y = m_orgY;
	int key = m_map->grid2key(startNode->point);

	m_openList.push_back(startNode);
	make_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
	m_openNodes.insert(pair<int,Node*>(key, startNode));

	while (!m_openList.empty())
	{
		// �ڿ����б���Ѱ��Fֵ��С�Ľڵ�;
		pop_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
		auto node = m_openList.back();

		// ���ýڵ�ӿ����б����Ƴ����������б�;
		m_openList.pop_back();
		m_openNodes.erase(key);
		m_closeNodes.insert(key);

		// �ҵ�·��������;
		if ( node->point.x==m_desX && node->point.y==m_desY )
		{
			m_path.clear();
			do {
				m_path.push_back(MapGrid(node->point.x, node->point.y));
				node = node->parent;
			} while ( node && node->parent ); // �ӵ���ʼ�ĵ�;
			_clearNodes();
			return true;
		}

		// �����ٽ�8�������;
		// 1  2  3
		// 4     6
		// 7  8  9
		{
			_checkChild(node->point.x+Fixed(1), node->point.y, node);		// 6
			_checkChild(node->point.x, node->point.y-Fixed(1),	node);		// 2
			_checkChild(node->point.x, node->point.y+Fixed(1), node);		// 8
			_checkChild(node->point.x-Fixed(1), node->point.y-Fixed(1), node);	// 4
		}



		//_checkChild(node->point.x+1, node->point.y-1, node);	// 3
		//_checkChild(node->point.x+1, node->point.y+1, node);	// 9
		//_checkChild(node->point.x-1, node->point.y, node);	// 1
		//_checkChild(node->point.x-1, node->point.y+1, node);	// 7
	}

	// δ�ҵ�;
	_clearNodes();
	return false;
}

bool PathFinder::_checkChild( const Fixed &x, const Fixed &y, Node *parent )
{
	// �ڵ㲻�ɴ�;
	if ( x<Fixed(0) || x>=m_mapW || y<Fixed(0) || y>=m_mapH || 
		m_map->isGridBarrier(x, y) ||
		(m_map->getGridRef(x,y)>=m_gridMaxContainCount && (x!=m_desX||y!=m_desY)) ) 
	{
		return true;
	}

	// ����Gֵ;
	Fixed G;
	if ( parent->point.x!=x && parent->point.y!=y ) G = parent->G + CORNER_COST;
	else G = parent->G + SIDE_COST;

	int key = m_map->grid2key(x, y);

	// ����Ƿ���ڷ���б��У��ڷ���б�����ʲô������;
	auto itClose = m_closeNodes.find(key);
	if (itClose != m_closeNodes.end())
	{
		return true;
	}

	// ����Ƿ���ڿ����б���;
	auto it = m_openNodes.find(key);
	if (it != m_openNodes.end())
	{
		// �Ѵ����ڿ����б��У��ж���parentNode����ýڵ��Gֵ�Ƿ�С��ԭ�������Gֵ;
		// ���С�ڣ������øýڵ���¸��׽ڵ�;
		// �����û����һ�Σ�Ҳ��Ѱ�ҵ�·�������ǲ����������̵ĵ�����;
		PathFinder::Node *node = it->second;
		if ( G < node->G ) {
			node->F -= (node->G-G);
			node->G = G;
			node->parent = parent;
			make_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
		}
	}
	else 
	{
		// �����ڿ����б��У������;
		PathFinder::Node *node = new PathFinder::Node;
		if ( node == NULL ) { _ASSERT(false); return false; }
		node->point.x = x;
		node->point.y = y;
		node->G = G;
		node->H = _calcH(node->point.x, node->point.y, m_desX, m_desY);
		node->F = node->G + node->H;
		node->parent = parent;
		m_openList.push_back(node);
		push_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
		m_openNodes.insert(pair<int,Node*>(key, node));
	}

	return true;

}

void PathFinder::_clearNodes()
{
	for (auto node : m_openList)
	{
		if (node) delete node;
	}
	m_openList.clear();

	m_openNodes.clear();
	m_closeNodes.clear();
}


