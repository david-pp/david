#include "LoginQueue.h"

const std::string LoginQueue::makeQueueKey(const std::string& queue)
{ 
	return getZoneName() + ":" + queue; 
}

const std::string LoginQueue::makeEnterKey(PlayerID id) 
{ 
	return getZoneName() + ":enter:" + boost::lexical_cast<std::string>(id);
}

const std::string LoginQueue::makeQueryKey(PlayerID id)
{
	return getZoneName() + ":query:" + boost::lexical_cast<std::string>(id);
}

LoginQueue& LoginQueue::instance()
{
	static LoginQueue queue;
	return queue;
}

int LoginQueue::checkEnterAndQueued(PlayerID id)
{
	if (!isOpen()) return 1;

	popUserInQueue();

	checkAndDelEnteringQueue();

	if (getWaitingQueueNum() >= getMaxQueueNum())
		return -1;

	if (!canEnterGame(id))
	{
		pushUserInQueue(id);
		return 0;
	}

	return 1;
}

bool LoginQueue::canEnterGame(PlayerID id)
{
	// 尚未满
	if ((getOnlineNum() + getTotalQueueNum()) < getMaxOnlineNum())
		return true;

	// 已经排到了
	if (redis.exists(makeEnterKey(id)))
		return true;

	return false;
}

void LoginQueue::pushUserInQueue(PlayerID id)
{
	// 进入等待队列
	redis.lrem(makeQueueKey(LOGIN_QUEUE_WAITING), id);
	redis.rpush(makeQueueKey(LOGIN_QUEUE_WAITING), id);

	redis.set(makeQueryKey(id), id);
	redis.expire(makeQueryKey(id), getLeaveTimeOut());
}

void LoginQueue::popUserInQueue()
{
	const uint32_t maxonlinenum = getMaxOnlineNum();
	const uint32_t waitingnum = redis.llen(makeQueueKey(LOGIN_QUEUE_WAITING));
	const uint32_t onlinenum = getOnlineNum() + redis.llen(makeQueueKey(LOGIN_QUEUE_ENTERING));
	const uint32_t emptynum = (maxonlinenum > onlinenum ? maxonlinenum - onlinenum : 0);

	for (uint32_t i = 0; i < emptynum && i < waitingnum; i++)
	{
		// 从WAITING队列到ENTERING队列
		uint32_t id = 0;
		if (redis.lpop(makeQueueKey(LOGIN_QUEUE_WAITING), id))
		{
			// 放入ENTERING队列
			redis.rpush(makeQueueKey(LOGIN_QUEUE_ENTERING), id);

			// 该玩家可以登录游戏了(一定时间没有操作就当做弃权)
			Json::Value player;
			player["id"] = id;
			player["enteringtime"] = (uint32_t)time(NULL);
			redis.set(makeEnterKey(id), player);

			// 设置有效时间
			redis.expire(makeEnterKey(id), getEnteringTimeOut());

			std::cout << "WATING->ENTERING:" << id << std::endl;
		}
	}
}

void LoginQueue::delUserInQueue(PlayerID id)
{
	redis.lrem(makeQueueKey(LOGIN_QUEUE_WAITING), id);
	redis.lrem(makeQueueKey(LOGIN_QUEUE_ENTERING), id);
	redis.del(makeEnterKey(id));
}

void LoginQueue::resetQueue()
{
	std::vector<std::string> keys;
	if (redis.keys(getZoneName() + ":*", keys))
	{
		for (size_t i = 0; i < keys.size(); i++)
			redis.del(keys[i]);
	}

	redis.lrem("serverlist", getZoneName());
	redis.rpush("serverlist", getZoneName());
}

void LoginQueue::checkAndDelEnteringQueue()
{
	std::vector<uint32_t> enterings;
	enterings.reserve(getMaxQueueNum());
	if (redis.lrange(makeQueueKey(LOGIN_QUEUE_ENTERING), 0, -1, enterings))
	{
		for (size_t i = 0; i < enterings.size(); i++)
		{
			if (!redis.exists(makeEnterKey(enterings[i])))
			{
				redis.lrem(makeQueueKey(LOGIN_QUEUE_ENTERING), enterings[i]);
			}
		}
	}
}

int LoginQueue::getTotalQueueNum()
{
	return redis.llen(makeQueueKey(LOGIN_QUEUE_WAITING)) + redis.llen(makeQueueKey(LOGIN_QUEUE_ENTERING));
}

int LoginQueue::getWaitingQueueNum()
{
	return redis.llen(makeQueueKey(LOGIN_QUEUE_WAITING));
}

int LoginQueue::getEnteringQueueNum()
{
	return redis.llen(makeQueueKey(LOGIN_QUEUE_ENTERING));
}

////////////////////////////////////////////////////////////

#ifdef _DEBUG_LOGINQUEUE
static std::set<uint32_t> onlineids;
#endif

uint32_t LoginQueue::getOnlineNum()
{
#ifdef _DEBUG_LOGINQUEUE
	return onlineids.size();
#endif

	return 100;
}

void LoginQueue::onStart()
{
	if (!isOpen()) return;
	
	redis.connect("127.0.0.1", 6379);

	resetQueue();
}

void LoginQueue::onTimer()
{
}

void LoginQueue::onUserEnterGame(PlayerID id)
{
	if (!isOpen()) return;

#ifdef _DEBUG_LOGINQUEUE
	onlineids.insert(id);
#endif

	delUserInQueue(id);

	redis.set(getZoneName()+":online", getOnlineNum());
}

void LoginQueue::onUserOffline(PlayerID id)
{
	if (!isOpen()) return;

#ifdef _DEBUG_LOGINQUEUE
	onlineids.erase(id);
#endif

	popUserInQueue();
	//checkAndDelEnteringQueue();

	redis.set(getZoneName()+":online", getOnlineNum());
}

