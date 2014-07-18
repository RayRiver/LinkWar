#include "MapStructs.h"

#include "Helper/Display.h"

MapGrid::MapGrid( int _x, int _y )
	: x(_x)
	, y(_y)
{

}

MapVec2::MapVec2( const Fixed &_x, const Fixed &_y )
	: x(_x)
	, y(_y)
{

}

MapVec2::MapVec2( const cocos2d::Vec2 &point )
	: x(point.x)
	, y(point.y)
{

}

cocos2d::Vec2 MapVec2::toPoint() const
{
	cocos2d::Vec2 point;
	point.x = x;
	point.y = y;
	return point;
}

void MapVec2::normalize()
{
	Fixed n = x * x + y * y;

	// Already normalized.
	if (n == Fixed::ONE)
	{
		return;
	}

	n = n.sqrt();

	// zero.
	if (n == Fixed::ZERO)
	{
		return;
	}

	n = Fixed::ONE / n;
	x *= n;
	y *= n;
}

MapRect::MapRect( const Fixed &_x, const Fixed &_y, const Fixed &_w, const Fixed &_h )
	: x(_x)
	, y(_y)
	, w(_w)
	, h(_h)
{

}

MapRect::MapRect(const cocos2d::Rect &rect)
	: x(rect.origin.x)
	, y(rect.origin.y)
	, w(rect.size.width)
	, h(rect.size.height)
{
	
}

cocos2d::Rect MapRect::toRect() const
{
	cocos2d::Rect rect;
	rect.origin.x = x;
	rect.origin.y = y;
	rect.size.width = w;
	rect.size.height = h;
	return rect;
}

bool MapRect::containsPoint(const MapPoint &point) const
{
	bool bRet = false;

	if (point.x >= x && point.x <= x+w
		&& point.y >= y && point.y <= y+h)
	{
		bRet = true;
	}

	return bRet;
}

