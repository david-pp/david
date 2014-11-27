
#include <iostream>
#include <deque>
#include "test.h"
#include "Redis.h"

void test_redis(int count)
{
	BLOCK_USAGETIME("redis", 0);

	RedisClient redis;

	{
		BLOCK_USAGETIME("redis-connect", 1);
		redis.connect("127.0.0.1", 6379);
	}

	{
		BLOCK_USAGETIME("redis-set", count);
		for (int i = 0; i < count; i++)
		{
			Json::Value player;
			player["id"] = i;
			player["enteringtime"] = (uint32_t)time(NULL);
			redis.set("utest:" + boost::lexical_cast<std::string>(i), player);
		}
	}
	{
		BLOCK_USAGETIME("redis-exists", count);
		for (int i = 0; i < count; i++)
		{
			redis.exists("utest:" + boost::lexical_cast<std::string>(i));
		}
	}
	{
		std::vector<std::string> keys;
		keys.reserve(count);

		BLOCK_USAGETIME("redis-keys", 1);
		redis.keys("utest:*", keys);
	}
	{
		BLOCK_USAGETIME("redis-expire", count);
		for (int i = 0; i < count; i++)
		{
			redis.expire("utest:" + boost::lexical_cast<std::string>(i), 1000);
		}
	}
	{
		BLOCK_USAGETIME("redis-del", count);
		for (int i = 0; i < count; i++)
		{
			redis.del("utest:" + boost::lexical_cast<std::string>(i));
		}
	}

	{
		BLOCK_USAGETIME("redis-rpush", count);
		for (int i = 0; i < count; i++)
		{
			redis.rpush("utest:list", i);
		}
	}
	{
		BLOCK_USAGETIME("redis-llen", count);
		for (int i = 0; i < count; i++)
		{
			redis.llen("utest:list");
		}
	}

	{
		std::vector<uint32_t> values;
		BLOCK_USAGETIME("redis-lrange", 100);
		for (int i = 0; i < 100; i++)
		{
			redis.lrange("utest:list", 0, -1, values);
		}
	}

	{
		std::vector<uint32_t> values;
		BLOCK_USAGETIME("redis-lrem", count);
		for (int i = 0; i < count; i++)
		{
			redis.lrem("utest:list", i);
		}
	}

	{
		BLOCK_USAGETIME("redis-rpush", count);
		for (int i = 0; i < count; i++)
		{
			redis.rpush("utest:list", i);
		}
	}
	{
		uint32_t value;
		BLOCK_USAGETIME("redis-lpop", count);
		for (int i = 0; i < count; i++)
		{
			redis.lpop("utest:list", value);
		}
	}

	redis.del("utest:list");
}

int main(int argc, char* argv[])
{
	if (argc > 1)
		test_redis(atol(argv[1]));
	else
		test_redis(1000);
	return 0;
}
