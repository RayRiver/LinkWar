#include "GameObjectView.h"

#include "Base/Fixed.h"
#include "Helper/Display.h"

#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"

USING_NS_CC;

const int RECT_POINTS = 4;

GameObjectView::GameObjectView()
	: m_shouldClean(false)
{

}

GameObjectView::~GameObjectView()
{

}

bool GameObjectView::init(int id)
{
	if (!Node::init())
	{
		return false;
	}

	m_id = id;

	this->setAnchorPoint(Vec2(0.5f, 0.5f));

	return true;
}

void GameObjectView::onInitialized()
{
	const auto object = GAME_OBJECT(m_id);

	// 显示受击盒;
	if (object->isShowHitBox())
	{
		auto drawNode = DrawNode::create();
		this->addChild(drawNode);
		if (drawNode)
		{
			// 获取对象受击盒;
			const auto &hitbox = object->getHitBox();

			// 转换成顶点列表;
			Vec2 verts[] = 
			{
				Vec2(hitbox.x, hitbox.y),
				Vec2(hitbox.x+hitbox.w, hitbox.y),
				Vec2(hitbox.x+hitbox.w, hitbox.y+hitbox.h),
				Vec2(hitbox.x, hitbox.y+hitbox.h)
			};

			// 绘制受击盒;
			switch (object->group())
			{
			case GameObjectGroup::Group0:
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 1.0f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
				break;
			case GameObjectGroup::Group1:
				drawNode->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 1.0f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
				break;
			default:
				break;
			}
		}
	}

	// 显示血条;
	m_hpBar = DrawNode::create();
	this->addChild(m_hpBar);
	this->updateHPBar();

	// 显示动画;
	m_armature = nullptr;
	auto animation_file = object->getAnimationFile();
	auto animation_name = object->getAnimationName();
	if (animation_file && animation_name)
	{
		cocostudio::ArmatureDataManager::getInstance()->addArmatureFileInfo(animation_file);
		auto armature = cocostudio::Armature::create(animation_name);
		if (armature)
		{
			this->addChild(armature);
		}

		// 默认起始动画;
		auto animation_default_action = object->getAnimationDefaultAction();
		if (animation_default_action)
		{
			// Default = -1,
			// Once = 0,
			// Loop = 1,
			armature->getAnimation()->play(animation_default_action, -1, -1);
		}

		// 动画拉伸比例;
		const auto &animation_scale = object->getAnimationScale();
		if (animation_scale != Fixed::ONE)
		{
			armature->setScale((float)animation_scale);
		}

		m_armature = armature;
	}

}

void GameObjectView::updateHPBar()
{
	const auto object = GAME_OBJECT(m_id);
	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(20, 30), 5.0f, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(20, 30), 4.0f, Color4F(0.0f, 0.0f, 0.0f, 1.0f));
	m_hpBar->drawSegment(Vec2(-20, 30), Vec2(-20+40*(float)(object->hp() / object->maxhp()), 30), 4.0f, Color4F(1.0f, 0.0f, 0.0f, 1.0f));
}

void GameObjectView::updateZOrder()
{
	float displayH = (float)(MAP->displayH());
	this->setLocalZOrder(MapManager::LAYER_MAP_NODE_BEGIN + (int)(displayH - this->getPositionY()));
}

void GameObjectView::playAnimation( const char *name )
{
	if (!m_armature)
	{
		log("armature not create");
		return;
	}

	m_armature->getAnimation()->play(name, -1, -1);
}

void GameObjectView::playAnimationWithDirection( const char *prefix )
{
	if (!m_armature)
	{
		log("armature not create");
		return;
	}

	auto direction = GAME_OBJECT(m_id)->getDirection();

	// 解析动画名;
	std::string name = prefix;
	switch (direction)
	{
	case GameObjectDirection::Left:
		name.append("_left");
		break;
	case GameObjectDirection::Right:
		name.append("_left");
		break;
	case GameObjectDirection::Up:
		name.append("_up");
		break;
	case GameObjectDirection::Down:
		name.append("_down");
		break;
	}

	// 解析方向;
	float scaleX = m_armature->getScaleX();
	if (direction == GameObjectDirection::Left && scaleX < 0)
	{
		m_armature->setScaleX(-scaleX);
	}
	else if (direction == GameObjectDirection::Right && scaleX > 0)
	{
		m_armature->setScaleX(-scaleX);
	}

	//m_armature->getAnimation()->play(name, -1, -1);
	playAnimation(name.c_str());
}

void GameObjectView::onIdle()
{
	playAnimationWithDirection("idle");
}

void GameObjectView::onMove()
{
	playAnimationWithDirection("move");
}

void GameObjectView::onAttack()
{
	playAnimationWithDirection("move");
}

void GameObjectView::onHit()
{
	playAnimationWithDirection("idle");
}

void GameObjectView::onDie()
{
	playAnimation("die");

	if (m_armature)
	{
		m_armature->getAnimation()->setMovementEventCallFunc([=](cocostudio::Armature *armature, cocostudio::MovementEventType movementType, const std::string& movementID) {
			if (movementType == cocostudio::MovementEventType::COMPLETE && movementID.compare("die") == 0)
			{
				m_armature->runAction(Sequence::create(
					DelayTime::create(0.5f),
					FadeOut::create(1.0f),
					CallFunc::create([=]{
						m_shouldClean = true;	
					}),
					nullptr
				));
			}
		});
	}
}

void GameObjectView::setColorMask( const Color3B &color )
{
	if (m_armature)
	{
		m_armature->setColor(color);
	}
}

