#include "BattleScene.h"

#include "Helper/Display.h"

#include "GameEntity.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTDebugRenderer.h"

#include "Helper/Display.h"

USING_NS_CC;

const int RECT_POINTS = 4;
const int GRID_SIZE = 40;

cocos2d::Scene * BattleScene::createScene()
{
	auto scene = BattleScene::create();
	return scene;
}

BattleScene::BattleScene()
{

}

BattleScene::~BattleScene()
{
	if (m_map)
	{
		delete m_map;
		m_map = nullptr;
	}
}

bool BattleScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	m_mapW = ((int)display.width()) / GRID_SIZE;
	m_mapH = ((int)display.height()) / GRID_SIZE;
	m_map = new unsigned char[m_mapW*m_mapH];
	memset(m_map, 0x00, m_mapW*m_mapH);

	this->createGrid();
	this->createLauncherArea();
	this->createBattleFieldArea();

	// touch event
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = [=](const std::vector<Touch*> &touches, Event *event) {
		for (const auto &touch : touches)
		{
			const Vec2 &point = touch->getLocation();
			if (this->m_selfLauncherArea.containsPoint(point))
			{
				// create soldier
				this->createEntity(false, point);

				Vec2 p;
				p.x = rand() % (int)(m_oppoLauncherArea.size.width) + (int)(m_oppoLauncherArea.origin.x);
				p.y = rand() % (int)(m_oppoLauncherArea.size.height) + (int)(m_oppoLauncherArea.origin.y);
				this->createEntity(true, p);
			}
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);


	this->scheduleUpdate();

	return true;
}

void BattleScene::update( float dt )
{
	// update map
	memset(m_map, 0x00, m_mapW*m_mapH);
	for (auto soldier : m_selfSoldiers)
	{
		const auto &pos = soldier->getPosition();
		auto x = (int)pos.x / GRID_SIZE;
		auto y = (int)pos.y / GRID_SIZE;
		m_map[y*m_mapW+x] = 0xff;
	}
	for (auto soldier : m_oppoSoldiers)
	{
		const auto &pos = soldier->getPosition();
		auto x = (int)pos.x / GRID_SIZE;
		auto y = (int)pos.y / GRID_SIZE;
		m_map[y*m_mapW+x] = 0xff;
	}
	

	std::set<GameEntity *> cleanSelfEntities;
	for (auto soldier : m_selfSoldiers)
	{
		if (soldier->shouldClean())
		{
			cleanSelfEntities.insert(soldier);
		}
		else
		{
			soldier->onFrame(dt);
		}
	}


	std::set<GameEntity *> cleanOppoEntities;
	for (auto soldier : m_oppoSoldiers)
	{
		if (soldier->shouldClean())
		{
			cleanOppoEntities.insert(soldier);
		}
		else
		{
			soldier->onFrame(dt);
		}
	}


	for (auto soldier : cleanSelfEntities)
	{
		m_selfSoldiers.erase(soldier);
		this->removeChild(soldier);
	}
	for (auto soldier : cleanOppoEntities)
	{
		m_oppoSoldiers.erase(soldier);
		this->removeChild(soldier);
	}
}

void BattleScene::createGrid()
{
	auto layer = LayerColor::create(Color4B(50, 50, 50, 255));
	this->addChild(layer, -10000);

	m_drawNode = DrawNode::create();
	this->addChild(m_drawNode);

	for (size_t i=0; i<display.width()/GRID_SIZE; ++i)
	{
		m_drawNode->drawSegment(Vec2(i*GRID_SIZE, display.top()), Vec2(i*GRID_SIZE, display.bottom()), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
	for (size_t i=0; i<display.height()/GRID_SIZE; ++i)
	{
		m_drawNode->drawSegment(Vec2(display.left(), i*GRID_SIZE), Vec2(display.right(), i*GRID_SIZE), 0.5f, Color4F(0.5f, 0.5f, 0.5f, 0.5f));
	}
}

void BattleScene::createLauncherArea()
{
	m_selfLauncherArea = Rect((display.width()-GRID_SIZE*14)/2, 0, GRID_SIZE*14, GRID_SIZE*4);
	m_oppoLauncherArea = Rect((display.width()-GRID_SIZE*14)/2, display.height()-GRID_SIZE*4, GRID_SIZE*14, GRID_SIZE*4);

	Vec2 selfVerts[RECT_POINTS], oppoVerts[RECT_POINTS];
	display.rect2points(m_selfLauncherArea, selfVerts);
	display.rect2points(m_oppoLauncherArea, oppoVerts);

	m_drawNode->drawPolygon(selfVerts, RECT_POINTS, Color4F(0.0f, 1.0f, 0.0f, 0.2f), 1.0f, Color4F(0.0f, 1.0f, 0.0f, 0.4f));
	m_drawNode->drawPolygon(oppoVerts, RECT_POINTS, Color4F(1.0f, 0.0f, 0.0f, 0.2f), 1.0f, Color4F(1.0f, 0.0f, 0.0f, 0.4f));

}

void BattleScene::createBattleFieldArea()
{
	auto x = (display.width()-GRID_SIZE*14)/2;
	auto y = GRID_SIZE*4;
	m_battleFieldArea = Rect(x, y, display.width()-2*x, display.height()-2*y);

	Vec2 verts[RECT_POINTS];
	display.rect2points(m_battleFieldArea, verts);
	m_drawNode->drawPolygon(verts, RECT_POINTS, Color4F(1.0f, 1.0f, 1.0f, 0.2f), 1.0f, Color4F(1.0f, 1.0f, 1.0f, 0.4f));
}

GameEntity * BattleScene::createEntity( bool isEnemy, const cocos2d::Vec2 &pos )
{
	auto soldier = GameEntity::create("config/entity_soldier.json", this, isEnemy);

	soldier->setPosition(pos);
	soldier->updateZOrder();
	this->addChild(soldier);

	if (isEnemy)
	{
		m_oppoSoldiers.insert(soldier);
	}
	else
	{
		m_selfSoldiers.insert(soldier);
	}

	return soldier;
}

void BattleScene::createBTDebugRenderer( BTNode *node )
{
	return;
	static bool isCreated = false;
	if (!isCreated)
	{
		auto renderer = BTDebugRenderer::create(node);
		this->addChild(renderer, 100000);
		isCreated = true;
	}
}

int BattleScene::getGridSize()
{
	return GRID_SIZE;
}


