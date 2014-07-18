#ifndef GameObjectView_h__
#define GameObjectView_h__

#include <string>

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"

#include "GameObjectMacros.h"

class Fixed;
class GameObjectView : public cocos2d::Node
{
public:
	GameObjectView();
	~GameObjectView();

	bool init(int id);

	// ��ʼ���ɹ���������ʾ����;
	void onInitialized();

	// �Ƿ���Ҫ����;
	inline bool shouldClean() { return m_shouldClean; }

	// ����Ѫ����ʾ;
	void updateHPBar();

	// ���¶���㼶;
	void updateZOrder();

	// ���Ŷ���;
	void playAnimationWithDirection(const char *prefix);
	void playAnimation(const char *name);

	// ������ɫ;
	void setColorMask(const cocos2d::Color3B &color);

	// ������Ϊ����;
	void onIdle();
	void onMove();
	void onAttack();
	void onHit();
	void onDie();

private:
	std::string getAnimationName(const char *prefix);

private:
	int m_id;
	cocos2d::DrawNode *m_hpBar;

	cocostudio::Armature *m_armature;

	bool m_shouldClean;
};

#endif // GameObjectView_h__
