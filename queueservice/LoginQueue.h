#ifndef __LOGIN_QUEUE_H
#define __LOGIN_QUEUE_H

#include <string>
#include "Redis.h"

#define LOGIN_QUEUE_WAITING  "qw"
#define LOGIN_QUEUE_ENTERING "qe"

///////////////////////////////////////////////////////////////
//
// 登录排队节制(2014/11/19 by David)
//
///////////////////////////////////////////////////////////////
class LoginQueue
{
	public:
		typedef int PlayerID;
		static LoginQueue& instance();

		// 排队机制开启?
		bool isOpen() { return true; }

		// 本区名
		const std::string getZoneName() { return "风雨同舟"; }

		// Entering队列超时(可进入游戏队列)
		uint32_t getEnteringTimeOut() { return 2; }
		// 判断玩家离开的判断时间
		uint32_t getLeaveTimeOut() { return 60; }

		// 当前在线人数、最大在线人数、WAITING队长
		uint32_t getOnlineNum();
		uint32_t getMaxOnlineNum() { return 10000; }
		uint32_t getMaxQueueNum() { return 10000; }

	public:
		//
		// 坚持玩家能否进入游戏，不能则排队
		// 返回: 
		//    1 - 可以进入游戏(该机制关闭时候也返回1)
		//    0 - 不能进入游戏、排队 
		//   -1 - 排队已满
		int checkEnterAndQueued(PlayerID id);

		// 服务器启动时触发
		void onStart();

		// 定时器触发
		void onTimer();

		// 玩家成功进入游戏时触发
		void onUserEnterGame(PlayerID id);

		// 玩家下线时触发
		void onUserOffline(PlayerID id);

	public:
		bool canEnterGame(PlayerID id);
		void pushUserInQueue(PlayerID id);
		void popUserInQueue();
		void delUserInQueue(PlayerID id);
		void resetQueue();
		void checkAndDelEnteringQueue();

		// 队列长度
		int getTotalQueueNum();
		int getWaitingQueueNum();
		int getEnteringQueueNum();

		// Redis相关KEY
		const std::string makeQueueKey(const std::string& queue); 
		const std::string makeEnterKey(PlayerID id);
		const std::string makeQueryKey(PlayerID id);

		RedisClient& getRedis() { return redis; }

	private:
		RedisClient redis;
};

#endif // __LOGIN_QUEUE_H
