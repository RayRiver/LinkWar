#include "LogicFrame.h"

#include "cocos2d.h"

#include "GameLogic.h"

using namespace std;

LogicFrameManager *LogicFrameManager::s_instance = nullptr;

LogicFrameManager * LogicFrameManager::getInstance()
{
	if (!s_instance)
	{
		s_instance = new LogicFrameManager;
	}
	return s_instance;
}

LogicFrameManager::LogicFrameManager()
	: m_currentFrame(nullptr)
	, m_currentFrameIndex(-1)
{

}

LogicFrameManager::~LogicFrameManager()
{
	clear();
}

void LogicFrameManager::init()
{
	clear();

	// TODO: 临时自动生成;
	cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);

}

void LogicFrameManager::clear()
{
	for (auto frame : m_frameQueue)
	{
		if (frame) delete frame;
	}
	m_frameQueue.clear();

	m_currentFrame = nullptr;
	m_currentFrameIndex = -1;
}

void LogicFrameManager::update( float dt )
{
	//m_frameQueue.push_back(nullptr);

	GameLogic::getInstance()->handleLogicFrame(nullptr);
}

void LogicFrameManager::addFrame( LogicFrame *frame )
{
	m_frameQueue.push_back(frame);
}

LogicFrame *LogicFrameManager::getFrame()
{
	if (m_frameQueue.size() == 0)
	{
		return nullptr;
	}

	if (m_currentFrame)
	{
		delete m_currentFrame;
		m_currentFrame = nullptr;
	}

	m_currentFrame = m_frameQueue.front();
	m_frameQueue.pop_front();

	++m_currentFrameIndex;

	return m_currentFrame;
}


