
#include <unistd.h>
#include <iostream>
#include <deque>
#include <limits>
#include "test.h"
#include "LoginQueue.h"

int freq = 1;
int onlinepercent = 70;
int enterpercent = 30;

inline bool randomByPercent(int percent)
{
	double r = random();
	r = r / numeric_limits<int>::max();
	r = r * 100;
	return percent > r;
}

void test_queue()
{
	srand(time(NULL));

	std::deque<int> onlines;

	int id = 0;
	LoginQueue::instance().onStart();
	while(true)
	{
		std::cout << (++id) <<  ". --- timer ---" << std::endl;
		LoginQueue::instance().onTimer();

		// 模拟上线
		if (randomByPercent(onlinepercent))
		{
			BLOCK_USAGETIME("Login", 0);
			int ret = LoginQueue::instance().checkEnterAndQueued(id);
			if (ret > 0)
			{
				LoginQueue::instance().onUserEnterGame(id);

				onlines.push_back(id);

				std::cout << "enter ... okay !" << std::endl;
			}
			else if (ret == 0)
			{
				std::cout << "enter ... queued !" << std::endl;
			}
			else
			{
				std::cout << "enter ... rejected!" << std::endl;
			}
		}
		// 模拟下线
		else
		{
			if (onlines.size() > 0)
			{
				LoginQueue::instance().onUserOffline(onlines.front());
				onlines.pop_front();

				std::cout << "offline ... okay ! " << std::endl;
			}
		}

		// 模拟排队中的玩家进入游戏
		std::vector<uint32_t> enterings;
		LoginQueue::instance().getRedis().lrange(
			LoginQueue::instance().makeQueueKey(LOGIN_QUEUE_ENTERING),
			0, -1,
			enterings);

		for (int i = 0; i < enterings.size(); i++)
		{
			if (randomByPercent(enterpercent))
			{
				uint32_t eid = enterings[i];

				BLOCK_USAGETIME("Login", 0);

				int ret = LoginQueue::instance().checkEnterAndQueued(eid);
				if (ret > 0)
				{
					LoginQueue::instance().onUserEnterGame(eid);

					onlines.push_back(eid);

					std::cout << "queued:enter ... okay !" << std::endl;
				}
				else if (ret == 0)
				{
					std::cout << "queued:enter ... queued !" << std::endl;
				}
				else
				{
					std::cout << "queued::enter ... rejected!" << std::endl;
				}
			}
		}
		enterings.clear();

		std::cout << "-- online:" << LoginQueue::instance().getOnlineNum() << std::endl;
		std::cout << "-- wating:" << LoginQueue::instance().getWaitingQueueNum() << std::endl;
		std::cout << "-- entering:" << LoginQueue::instance().getEnteringQueueNum() << std::endl;

		usleep(1000000/freq);
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
		freq = atol(argv[1]);
	if (argc > 2)
		onlinepercent = atol(argv[2]);
	if (argc > 3)
		enterpercent = atol(argv[3]);

	test_queue();
	return 0;
}

