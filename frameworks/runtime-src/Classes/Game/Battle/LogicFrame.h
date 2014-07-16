#ifndef LogicFrame_h__
#define LogicFrame_h__

#include <vector>
#include <deque>

struct LogicFrameAction
{
	enum class Type
	{
		CreateEntity = 1,
	};

	LogicFrameAction(int _uid, LogicFrameAction::Type _type, float _x, float _y, int _id) : uid(_uid), type(_type), x(_x), y(_y), id(_id) {}

	int uid;
	Type type;
	float x, y;
	int id;
};

struct LogicFrame
{
	std::vector<LogicFrameAction *> actions;
	~LogicFrame() {
		for (auto action : actions)
		{
			if (action) delete action;
		}
	}
};

class LogicFrameManager
{
public:
	static LogicFrameManager *getInstance();
	~LogicFrameManager();

	void init();
	void clear();

	void addFrame(LogicFrame *frame);
	LogicFrame *getFrame();

	void update(float dt);

	inline LogicFrame *getCurrentFrame() { return m_currentFrame; }
	inline int getCurrentFrameIndex() { return m_currentFrameIndex; }

private:	
	LogicFrameManager();
	static LogicFrameManager *s_instance;

	std::deque<LogicFrame *> m_frameQueue;

	LogicFrame *m_currentFrame;
	int m_currentFrameIndex;
};

#endif // LogicFrame_h__
