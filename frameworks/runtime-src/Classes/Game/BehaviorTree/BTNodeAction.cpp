#include "BTNodeAction.h"

#include "cocos2d.h"
#include "BTDebugRenderer.h"

BTNodeAction::BTNodeAction( BTPrecondition *precondition /*= nullptr*/ )
	: BTNode(precondition)
	//, m_isRunning(false)
	, m_state(BTActionStatus::Ready)
	, m_bNeedExit(false)
{

}

void BTNodeAction::onEnter( const BTInputParam &input )
{

}

void BTNodeAction::onExit( const BTInputParam &input, BTRunningStatus state )
{

}

BTRunningStatus BTNodeAction::onExecute( const BTInputParam &input, BTOutputParam &output )
{
	return BTRunningStatus::Finish;
}

void BTNodeAction::onTransition( const BTInputParam &input )
{
	//if (m_state == BTActionStatus::Running)
	if (m_bNeedExit)
	{
		doExit(input, BTRunningStatus::Terminal);
	}

	m_bNeedExit = false;
	m_state = BTActionStatus::Ready;


	/*
	if (m_isRunning)
	{
		doExit(input, BTRunningStatus::Terminal);
		m_isRunning = false;
	}
	*/
}

// 走到doTick表示必须开始执行
BTRunningStatus BTNodeAction::onUpdate( const BTInputParam &input, BTOutputParam &output )
{
	BTRunningStatus running_state = BTRunningStatus::Finish;

	if (m_state == BTActionStatus::Ready)
	{
		doEnter(input);
		m_bNeedExit = true;
		m_state = BTActionStatus::Running;
	}

	if (m_state == BTActionStatus::Running)
	{
		running_state = onExecute(input, output);
		if (running_state != BTRunningStatus::Executing)
		{
			m_state = BTActionStatus::Finish;
		}
	}

	if (m_state == BTActionStatus::Finish)
	{
		if (m_bNeedExit)
		{
			doExit(input, running_state);
		}
		m_bNeedExit = false;

		m_state = BTActionStatus::Ready;
	}

	return running_state;



	/*
	// 开始执行，调用enter方法
	if (!m_isRunning)
	{
		doEnter(input);
		m_isRunning = true;
	}

	BTRunningStatus running_state = onExecute(input, output);	
	if (running_state == BTRunningStatus::Executing)
	{
		// 执行未完成
		return running_state;
	}
	else
	{
		// 执行完成，调用exit方法
		m_isRunning = false;	
		doExit(input, running_state);
		return running_state;
	}
	*/

}

void BTNodeAction::doEnter( const BTInputParam &input )
{
	if (BTDebugRenderer::getInstance())
	{
		cocos2d::log("%s onEnter", this->m_name.c_str());
		BTDebugRenderer::getInstance()->renderNode(this, BTNodeEvent::enter);
	}

	onEnter(input);
}

void BTNodeAction::doExit( const BTInputParam &input, BTRunningStatus state )
{
	if (BTDebugRenderer::getInstance())
	{
		cocos2d::log("%s onExit", this->m_name.c_str());
		BTDebugRenderer::getInstance()->renderNode(this, BTNodeEvent::exit);
	}

	onExit(input, state);
}

