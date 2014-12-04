
#include <map>
#include <string>
#include <set>

/////////////////////////////////////////////////////////
//
// 消息统计模块 (by David 2014/12/04)
//
/////////////////////////////////////////////////////////

class MsgStatistics
{
public:
	MsgStatistics();

	//
	// 统计时间间隔(单位:s)
	//
	int getStatInterval() const  { return 1; }
	//
	// 消息总数的最大频率
	//
	int getTotalFreq() const { return 0; }
	//
	// 针对单个消息的最大频率
	// 
	int getMsgFreq(int msgid) const { return 0; }


	//
	// 检查消息合法性
	// 1   - OKAY
	// 0   - 消息总数太快
	// -1  - 该消息过快 
	//
	int isMsgOkay(int msgid);

	//
	// 统计消息
	//
	void statMsg(int msgid);

	//
	// Dump统计信息到日志
	//
	void dumpMsgs();

private:
	size_t getMsgCount(int msgid);

	typedef std::map<int, size_t> MsgMap;

	/// 统计时间点
	uint32_t stat_timepoint_;
	/// 消息容器
	MsgMap msgs_;
	/// 消息总数
	size_t totalmsgs_;
};

