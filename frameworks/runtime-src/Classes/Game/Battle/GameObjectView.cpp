#include "GameObjectView.h"

#include "Base/Fixed.h"
#include "Helper/Display.h"

#include "MapManager.h"
#include "GameObjectManager.h"
#include "GameObject.h"

USING_NS_CC;

#define GAME_OBJECT(id) GameObjectManager::getInstance()->getObject(id)

const int RECT_POINTS = 4;

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

	// ��ʾ�ܻ���;
	auto drawNode = DrawNode::create();
	this->addChild(drawNode);
	if (drawNode)
	{
		// ��ȡ�����ܻ���;
		const auto &hitbox = object->getHitBox();

		// ת���ɶ����б�;
		Vec2 verts[] = 
		{
			Vec2(hitbox.x, hitbox.y),
			Vec2(hitbox.x+hitbox.w, hitbox.y),
			Vec2(hitbox.x+hitbox.w, hitbox.y+hitbox.h),
			Vec2(hitbox.x, hitbox.y+hitbox.h)
		};

		// �����ܻ���;
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

	// ��ʾѪ��;
	m_hpBar = DrawNode::create();
	this->addChild(m_hpBar);
	this->updateHPBar();

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
