#include "AStar.h"

#include <math.h>

USING_NS_CC;
using namespace std;

#define CORNER_COST		1.414f
#define SIDE_COST		1.0f

// 二叉堆比较算法（以F值比较，小顶堆）
template <typename T>
class MinBinaryHeapCompare
{
public:
	bool operator() ( T a, T b ) const
	{
		return a->F>b->F;
	}
};

// 曼哈顿法计算H值
static double _calcH(int x1, int y1, int x2, int y2)
{
	return (fabs((double)(x2-x1))+fabs((double)(y2-y1))) * SIDE_COST;
}

AStar::AStar()
	: m_map(nullptr)
	, m_mapW(0)
	, m_mapH(0)
	, m_orgX(-1)
	, m_orgY(-1)
	, m_desX(-1)
	, m_desY(-1)
{

}

AStar::~AStar()
{
	_clearNodes();	
}

void AStar::loadMap( unsigned char * map, unsigned int w, unsigned int h )
{
	m_map = map;
	m_mapW = w;
	m_mapH = h;
}

void AStar::setOrigin( const cocos2d::Vec2 &point )
{
	m_orgX = point.x;
	m_orgY = point.y;
}

void AStar::setDestination( const cocos2d::Vec2 &point )
{
	m_desX = point.x;
	m_desY = point.y;
}

bool AStar::findPath()
{
	if (m_map==nullptr || m_mapW==0 || m_mapH==0 || 
		m_orgX<0 || m_orgY<0 || m_orgX>=m_mapW || m_orgY>=m_mapH ||
		m_desX<0 || m_desY<0 || m_desX>=m_mapW || m_desY>=m_mapH)
	{ 
		CCASSERT(false, ""); 
		return false; 
	}

	// 初始节点加入开放列表
	Node *startNode = new Node;
	startNode->point.x = m_orgX;
	startNode->point.y = m_orgY;
	int key = _vec2key(startNode->point);

	m_openList.push_back(startNode);
	make_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
	m_openNodes.insert(pair<int,Node*>(key, startNode));

	while (!m_openList.empty())
	{
		// 在开放列表中寻找F值最小的节点
		pop_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
		auto node = m_openList.back();

		// 将该节点从开放列表中移除，加入封闭列表
		m_openList.pop_back();
		m_openNodes.erase(key);
		m_closeNodes.insert(key);

		// 找到路径，回溯
		if ( node->point.x==m_desX && node->point.y==m_desY )
		{
			m_path.clear();
			do {
				m_path.push_back(Vec2(node->point.x, node->point.y));
				node = node->parent;
			} while ( node->parent );
			_clearNodes();
			return true;
		}

		// 测试临近8点的属性
		// 1  2  3
		// 4     6
		// 7  8  9
		_checkChild(node->point.x+1, node->point.y, node);		// 6
		_checkChild(node->point.x+1, node->point.y-1, node);	// 3
		_checkChild(node->point.x+1, node->point.y+1, node);	// 9
		_checkChild(node->point.x, node->point.y-1,	node);		// 2
		_checkChild(node->point.x, node->point.y+1, node);		// 8
		_checkChild(node->point.x-1, node->point.y, node);		// 1
		_checkChild(node->point.x-1, node->point.y-1, node);	// 4
		_checkChild(node->point.x-1, node->point.y+1, node);	// 7
	}

	// 未找到
	_clearNodes();
	return false;
}

bool AStar::_checkChild( int x, int y, Node *parent )
{
	// 节点不可达
	if ( x<0 || (unsigned int)x>=m_mapW || y<0 || (unsigned int)y>=m_mapH || (m_map[y*m_mapW+x]!=0 && (x!=m_desX||y!=m_desY)) ) return true;

	// 计算G值
	double G = 0.0f;
	if ( parent->point.x!=x && parent->point.y!=y ) G = parent->G + CORNER_COST;
	else G = parent->G + SIDE_COST;

	int key = _xy2key(x, y);

	// 检查是否存在封闭列表中，在封闭列表中则什么都不做
	auto itClose = m_closeNodes.find(key);
	if (itClose != m_closeNodes.end())
	{
		return true;
	}

	// 检查是否存在开放列表中
	auto it = m_openNodes.find(key);
	if (it != m_openNodes.end())
	{
		// 已存在于开放列表中，判断由parentNode到达该节点的G值是否小于原来保存的G值
		// 如果小于，则设置该节点的新父亲节点
		// （如果没有这一段，也能寻找到路径，但是不会做相对最短的调整）
		Node *node = it->second;
		if ( G < node->G ) {
			node->F -= (node->G-G);
			node->G = G;
			node->parent = parent;
			make_heap(m_openList.begin(), m_openList.end(), MinBinaryHeapCompare<Node *>());
		}
	}
	else 
	{
		// 不存在开放列表中，则加入
		Node *node = new Node;
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

void AStar::_clearNodes()
{
	for (auto node : m_openList)
	{
		if (node) delete node;
	}
	m_openList.clear();

	m_openNodes.clear();
	m_closeNodes.clear();
}


