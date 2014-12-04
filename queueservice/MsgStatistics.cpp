#include "MsgStatistics.h"
#include <iostream>


MsgStatistics::MsgStatistics()
{
	stat_timepoint_ = time(NULL);
	totalmsgs_ = 0;
}

size_t MsgStatistics::getMsgCount(int msgid)
{
	MsgMap::iterator it = msgs_.find(msgid);
	if (it != msgs_.end())
		return it->second;
	return 0;
}

int MsgStatistics::isMsgOkay(int msgid)
{
	if (getTotalFreq() > 0 && totalmsgs_ >= getTotalFreq())
		return 0;

	int msgfre = getMsgFreq(msgid);
	if (msgfre > 0 && getMsgCount(msgid) >= msgfre)
		return -1;

	return 1;
}

void MsgStatistics::statMsg(int msgid)
{
	msgs_[msgid] ++;
	totalmsgs_ ++;

	uint32_t now = time(NULL);
	if (now > (stat_timepoint_ + this->getStatInterval()))
	{
		dumpMsgs();
		stat_timepoint_ = now;
	}
}

struct MsgSortInfo
{
public:
	bool operator < (const MsgSortInfo& rhs) const 
	{
		if (count != rhs.count)
			return count > rhs.count;
		return msgid < rhs.msgid;
	}
public:
	int msgid;
	size_t count;
};

typedef std::multiset<MsgSortInfo>  MsgSortInfoSet;

void MsgStatistics::dumpMsgs()
{
	std::cout << "[消息统计] 总数:" << totalmsgs_ << "/" << getStatInterval() << "s" << std::endl;

	MsgSortInfoSet sort;
	for (MsgMap::iterator it = msgs_.begin(); it != msgs_.end(); ++it)
	{
		MsgSortInfo info;
		info.msgid = it->first;
		info.count = it->second;	
		sort.insert(info);
	}

	for (MsgSortInfoSet::iterator it = sort.begin(); it != sort.end(); ++ it)
	{
		std::cout << "[消息统计] " << it->msgid << " - " << it->count << std::endl;
	}

	msgs_.clear();
	totalmsgs_ = 0;	
}
