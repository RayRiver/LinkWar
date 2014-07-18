#ifndef MapGrid_h__
#define MapGrid_h__

#include "cocos2d.h"

#include "Base/Fixed.h"

// 地图格子;
class MapGrid
{
public:
	enum class Type
	{
		None = 0,
		Barrier = 0xff,
	};

	Fixed x, y;

	MapGrid(int _x = -1, int _y = -1);

	inline bool operator==(const MapGrid &other) {
		return x==other.x && y==other.y;
	}

	inline bool operator!=(const MapGrid &other) {
		return x!=other.x || y!=other.y;
	}	

	inline void operator=(const MapGrid &other) {
		x = other.x;
		y = other.y;
	}
};

// 地图格子数据;
class MapGridData
{
public:
	MapGrid grid;
	MapGrid::Type type;	
	int refCount;

	MapGridData() : type(MapGrid::Type::None), refCount(0) {} 
};

// 地图向量;
class MapVec2
{
public:
	Fixed x, y;

	MapVec2() {}
	MapVec2(const Fixed &_x, const Fixed &_y);
	MapVec2(const cocos2d::Vec2 &point);

	cocos2d::Vec2 toPoint() const;

	void normalize();	

	inline Fixed length() const {
		return (x * x + y * y).sqrt();
	}

	inline Fixed lengthSquared() const {
		return (x * x + y * y);
	}

	inline bool operator==(const MapVec2 &other) const {
		return x==other.x && y==other.y;
	}
	inline bool operator!=(const MapVec2 &other) const {
		return x!=other.x || y!=other.y;
	}
	inline MapVec2 operator+(const MapVec2 &other) const {
		MapVec2 result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}
	inline MapVec2 operator-(const MapVec2 &other) const {
		MapVec2 result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}
};

// 地图向量;
typedef MapVec2 MapPoint;

// 地图区域;
class MapRect
{
public:
	Fixed x, y, w, h;

	MapRect() {}
	MapRect(const Fixed &_x, const Fixed &_y, const Fixed &_w, const Fixed &_h);
	MapRect(const cocos2d::Rect &rect);

	cocos2d::Rect toRect() const;
	bool containsPoint(const MapPoint &point) const;

	inline void operator=(const MapRect &other) {
		x = other.x;
		y = other.y;
		w = other.w;
		h = other.h;
	}
};

#endif // MapGrid_h__

