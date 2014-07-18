#ifndef GameObjectView_h__
#define GameObjectView_h__

#include "cocos2d.h"

class Fixed;
class GameObjectView : public cocos2d::Node
{
public:
	bool init(int id);

	// 初始化成功，根据显示内容;
	void onInitialized();

	// 更新血条显示;
	void updateHPBar();

	// 更新对象层级;
	void updateZOrder();

private:
	int m_id;
	cocos2d::DrawNode *m_hpBar;

};

#endif // GameObjectView_h__
