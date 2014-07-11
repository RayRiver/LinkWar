#ifndef DebugInfo_h__
#define DebugInfo_h__

#include <string>
#include <map>

#include "cocos2d.h"

class DebugInfo
{
public:
	static DebugInfo *getInstance();

	void init(cocos2d::Node *node);
	void show(bool bVisible = true);

	void setString(const char *name, const char *val);
	const char *getString(const char *name);
	void setBoolean(const char *name, bool val);
	bool getBoolean(const char *name);
	void setInt(const char *name, int val);
	int getInt(const char *name);
	void setFloat(const char *name, float val);
	float getFloat(const char *name);


public:
	~DebugInfo();

private:
	DebugInfo(); 
	static DebugInfo *s_instance;
	cocos2d::Layer *m_layer;
	bool m_bVisible;

	struct Info
	{
		std::string name;
		cocos2d::Label *label;
		union
		{
			bool booleanVal;
			int intVal;
			float floatVal;
		};
		std::string stringVal;
	};
	typedef std::map<std::string, Info> INFO_MAP;
	typedef std::pair<std::string, Info> INFO_MAP_PAIR;
	INFO_MAP m_infoMap;

	float m_currentHeight;

	Info *find(const char *name);
	void createLabel(Info *info);
};

#ifdef _DEBUG
# define DEBUGINFO_INIT(node) DebugInfo::getInstance()->init((node))
# define DEBUGINFO_SETSTRING(name, val) DebugInfo::getInstance()->setString((name), (val))
# define DEBUGINFO_GETSTRING(name) DebugInfo::getInstance()->getString((name))
# define DEBUGINFO_SETBOOLEAN(name, val) DebugInfo::getInstance()->setBoolean((name), (val))
# define DEBUGINFO_GETBOOLEAN(name) DebugInfo::getInstance()->getBoolean((name))
# define DEBUGINFO_SETINT(name, val) DebugInfo::getInstance()->setInt((name), (val))
# define DEBUGINFO_GETINT(name) DebugInfo::getInstance()->getInt((name))
# define DEBUGINFO_SETFLOAT(name, val) DebugInfo::getInstance()->setFloat((name), (val))
# define DEBUGINFO_GETFLOAT(name) DebugInfo::getInstance()->getFloat((name))
#else
# define DEBUGINFO_INIT(node)
# define DEBUGINFO_SETSTRING(name, val) 
# define DEBUGINFO_GETSTRING(name) nullptr
# define DEBUGINFO_SETBOOLEAN(name, val) 
# define DEBUGINFO_GETBOOLEAN(name) false
# define DEBUGINFO_SETINT(name, val) 
# define DEBUGINFO_GETINT(name) 0
# define DEBUGINFO_SETFLOAT(name, val) 
# define DEBUGINFO_GETFLOAT(name) 0.0f
#endif

#endif // DebugInfo_h__
