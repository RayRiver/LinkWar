#ifndef MapView_h__
#define MapView_h__

#include "cocos2d.h"

#include "Base/Fixed.h"

class MapView : public cocos2d::Layer
{
public:
	MapView();
	~MapView();

	static MapView *create();
	bool init();

	void createBg();
	void createGrid();
	void createLauncherArea();
	void createBattleFieldArea();
	void createTerrain();

private:
	Fixed m_mapW, m_mapH;
	Fixed m_gridW, m_gridH;
	Fixed m_displayW, m_displayH;

	cocos2d::Touch *m_draggingTouch;
	float m_draggingMoveDistance2;
};

#endif // MapView_h__
