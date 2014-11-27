#ifndef __REDIS_CLIENT_H
#define __REDIS_CLIENT_H

#include <string>
#include "redisclient.h"
#include "json/json.h"

class RedisClient
{
	public:
		RedisClient();
		~RedisClient();

		bool connect(const std::string& host, uint16_t port);
		bool reconnect();
		void close();

		bool isConnected() { return redis_cli != NULL; }

		redis::client* operator -> ()
		{
			if (!redis_cli) 
				reconnect();

			if (!redis_cli)
				throw redis::connection_error("redis is not connected !!");

			return redis_cli;
		}

	public:
		int  keys(const std::string& keypattern, std::vector<std::string>& keys);
		bool exists(const std::string& key);
		bool expire(const std::string& key, int seconds);
		bool del(const std::string& key);

		bool set(const std::string& key, int value);
		bool set(const std::string& key, Json::Value& value);
		bool get(const std::string& key, Json::Value& value);

		int  llen(const std::string& key);
		int  lrange(const std::string& key, int start, int end, std::vector<uint32_t>& values);
		int  rpush(const std::string& key, uint32_t value);
		int  rpush(const std::string& key, const std::string& value);
		bool lpop(const std::string& key, uint32_t& value);
		int  lrem(const std::string& key, uint32_t value);
		int  lrem(const std::string& key, const std::string& value);

	private:
		redis::client* redis_cli;
		std::string host;
		uint16_t port;
};

#endif // __REDIS_CLIENT_H
