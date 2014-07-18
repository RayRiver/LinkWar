#include "DebugInfo.h"

#include "Helper/Display.h"

USING_NS_CC;

DebugInfo *DebugInfo::s_instance = nullptr;

DebugInfo * DebugInfo::getInstance()
{
	if (!s_instance)	
	{
		s_instance = new DebugInfo;	
	}
	return s_instance;
}

DebugInfo::DebugInfo()
	: m_layer(nullptr)
	, m_bVisible(false)
	, m_currentHeight(0.0f)
{

}

DebugInfo::~DebugInfo()
{

}

void DebugInfo::init( cocos2d::Node * node )
{
	m_layer = Layer::create();
	m_layer->setVisible(false);
	m_bVisible = false;

	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = [=](EventKeyboard::KeyCode keycode, Event *event) {
		if (keycode == EventKeyboard::KeyCode::KEY_F12)
		{
			this->show(!m_bVisible);
		}
	};
	m_layer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_layer);

	node->addChild(m_layer, 999999);

	// create title
	auto label = Label::createWithSystemFont("DebugInfo", "Arial", 18);
	label->setAnchorPoint(Vec2(0,1));
	label->setPosition(Vec2(0, DisplayHelper::getInstance()->height()-m_currentHeight));
	m_layer->addChild(label);
	m_currentHeight += label->getContentSize().height;
}

void DebugInfo::show( bool bVisible /*= true*/ )
{
	m_bVisible = bVisible;
	m_layer->setVisible(m_bVisible);
}

void DebugInfo::setString( const char *name, const char *val )
{
	CCASSERT(m_layer, "");

	auto info = find(name);
	if (info)
	{
		info->stringVal = val;
		info->label->setString(info->name + ": " + info->stringVal);
	}
	else
	{
		Info info;
		info.name = name;
		info.stringVal = val;
		createLabel(&info);
		m_infoMap.insert(INFO_MAP_PAIR(name, info));
	}
}

const char * DebugInfo::getString( const char *name )
{
	auto info = find(name);
	if (info)
	{
		return info->stringVal.c_str();
	}
	else
	{
		return nullptr;
	}
}

void DebugInfo::setBoolean( const char *name, bool val )
{
	CCASSERT(m_layer, "");

	auto info = find(name);
	if (info)
	{
		info->booleanVal = val;
		info->stringVal = val ? "true" : "false";
		info->label->setString(info->name + ": " + info->stringVal);
	}
	else
	{
		Info info;
		info.name = name;
		info.booleanVal = val;
		info.stringVal = val ? "true" : "false";
		createLabel(&info);
		m_infoMap.insert(INFO_MAP_PAIR(name, info));
	}
}

bool DebugInfo::getBoolean( const char *name )
{
	auto info = find(name);
	if (info)
	{
		return info->booleanVal;
	}
	else
	{
		return false;
	}
}

void DebugInfo::setInt( const char *name, int val )
{
	CCASSERT(m_layer, "");

	auto info = find(name);
	if (info)
	{
		info->intVal = val;
		char s[32];
		sprintf(s, "%d", val);
		info->stringVal = s;
		info->label->setString(info->name + ": " + info->stringVal);
	}
	else
	{
		Info info;
		info.name = name;
		info.intVal = val;
		char s[32];
		sprintf(s, "%d", val);
		info.stringVal = s;
		createLabel(&info);
		m_infoMap.insert(INFO_MAP_PAIR(name, info));
	}
}

int DebugInfo::getInt( const char *name )
{
	auto info = find(name);
	if (info)
	{
		return info->intVal;
	}
	else
	{
		return 0;
	}
}

void DebugInfo::setFloat( const char *name, float val )
{
	CCASSERT(m_layer, "");

	auto info = find(name);
	if (info)
	{
		info->floatVal = val;
		char s[32];
		sprintf(s, "%f", val);
		info->stringVal = s;
		info->label->setString(info->name + ": " + info->stringVal);
	}
	else
	{
		Info info;
		info.name = name;
		info.floatVal = val;
		char s[32];
		sprintf(s, "%f", val);
		info.stringVal = s;
		createLabel(&info);
		m_infoMap.insert(INFO_MAP_PAIR(name, info));
	}
}

float DebugInfo::getFloat( const char *name )
{
	auto info = find(name);
	if (info)
	{
		return info->floatVal;
	}
	else
	{
		return 0.0f;
	}
}

DebugInfo::Info * DebugInfo::find( const char *name )
{
	auto it = m_infoMap.find(name);
	if (it != m_infoMap.end())
	{
		return &(it->second);
	}
	else
	{
		return nullptr;
	}
}

void DebugInfo::createLabel( DebugInfo::Info *info )
{
	info->label = Label::createWithSystemFont(info->name + ": " + info->stringVal, "Arial", 18);

	info->label->setAnchorPoint(Vec2(0,1));
	info->label->setPosition(Vec2(0, DisplayHelper::getInstance()->height()-m_currentHeight));
	m_layer->addChild(info->label);

	m_currentHeight += info->label->getContentSize().height;
}
