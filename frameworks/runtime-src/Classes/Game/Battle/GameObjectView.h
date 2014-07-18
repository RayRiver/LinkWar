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

	// 初始化成功，根据显示内容;
	void onInitialized();

	// 是否需要清理;
	inline bool shouldClean() { return m_shouldClean; }

	// 更新血条显示;
	void updateHPBar();

	// 更新对象层级;
	void updateZOrder();

	// 播放动画;
	void playAnimationWithDirection(const char *prefix);
	void playAnimation(const char *name);

	// 设置颜色;
	void setColorMask(const cocos2d::Color3B &color);

	// 对象行为表现;
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
