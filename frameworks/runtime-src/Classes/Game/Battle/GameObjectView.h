#ifndef GameObjectView_h__
#define GameObjectView_h__

#include "cocos2d.h"

class Fixed;
class GameObjectView : public cocos2d::Node
{
public:
	bool init(int id);

	// ��ʼ���ɹ���������ʾ����;
	void onInitialized();

	// ����Ѫ����ʾ;
	void updateHPBar();

	// ���¶���㼶;
	void updateZOrder();

private:
	int m_id;
	cocos2d::DrawNode *m_hpBar;

};

#endif // GameObjectView_h__
