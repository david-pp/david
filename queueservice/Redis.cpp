#include "Redis.h"

RedisClient::RedisClient()
{
	redis_cli = NULL;
	port = 0;
}

RedisClient::~RedisClient()
{
	this->close();
}

bool RedisClient::connect(const std::string& host, uint16_t port)
{
	this->host = host;
	this->port = port;
	this->close();
	
	try 
	{
		redis_cli = new redis::client(host, port);
		return true;
	}
	catch (redis::redis_error& e) 
	{
		redis_cli = NULL;
		std::cerr << "error:" << e.what() << std::endl;
		return false;
	}

	return false;
}

bool RedisClient::reconnect()
{
	return connect(this->host, this->port);
}

void RedisClient::close()
{
	if (redis_cli)
	{
		delete redis_cli;
		redis_cli = NULL;
	}
}

#define REDIS_TRY try 
#define REDIS_CATCH catch (redis::redis_error& e) { \
		close(); \
		std::cerr << "error:" << e.what() << std::endl; \
	}

int  RedisClient::keys(const std::string& keypattern, std::vector<std::string>& keys)
{
	REDIS_TRY 
	{
		return (*this)->keys(keypattern, keys);
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::exists(const std::string& key)
{
	REDIS_TRY 
	{
		return (*this)->exists(key);
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::expire(const std::string& key, int seconds)
{
	REDIS_TRY
	{
		(*this)->expire(key, seconds);
		return true;
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::del(const std::string& key)
{
	REDIS_TRY
	{
		(*this)->del(key);
		return true;
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::set(const std::string& key, int value)
{
	REDIS_TRY
	{
		(*this)->set(key, boost::lexical_cast<std::string>(value));
		return true;
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::set(const std::string& key, Json::Value& value)
{
	REDIS_TRY
	{
		(*this)->set(key, Json::FastWriter().write(value));
		return true;
	}
	REDIS_CATCH;
	return false;
}

bool RedisClient::get(const std::string& key, Json::Value& value)
{
	REDIS_TRY
	{
		std::string ret = (*this)->get(key);
		if (ret != redis::client::missing_value())
		{
			Json::Reader().parse(ret, value);
			return true;
		}
	}
	REDIS_CATCH;
	return false;
}

int RedisClient::llen(const std::string& key)
{
	REDIS_TRY
	{
		return (*this)->llen(key);
	}
	REDIS_CATCH;
	return 0;
}

int RedisClient::lrange(const std::string& key, int start, int end, std::vector<uint32_t>& values)
{
	REDIS_TRY
	{
		std::vector<std::string> rets;
		(*this)->lrange(key, start, end, rets);

		for (size_t i = 0; i < rets.size(); i++)
			values.push_back(boost::lexical_cast<uint32_t>(rets[i]));

		return rets.size();
	}
	REDIS_CATCH;
	return 0;
}

int RedisClient::rpush(const std::string& key, uint32_t value)
{
	REDIS_TRY
	{
		return (*this)->rpush(key, boost::lexical_cast<std::string>(value));
	}
	REDIS_CATCH;
	return 0;
}

int RedisClient::rpush(const std::string& key, const std::string& value)
{
	REDIS_TRY
	{
		return (*this)->rpush(key, value);
	}
	REDIS_CATCH;
	return 0;
}

bool RedisClient::lpop(const std::string& key, uint32_t& value)
{
	REDIS_TRY
	{
		std::string ret = (*this)->lpop(key);
		if (!ret.empty() && ret != redis::client::missing_value())
		{
			value = boost::lexical_cast<uint32_t>(ret);
			return true;
		}
	}
	REDIS_CATCH;
	return false;
}

int RedisClient::lrem(const std::string& key, uint32_t value)
{
	REDIS_TRY
	{
		return (*this)->lrem(key, 0, boost::lexical_cast<std::string>(value));
	}
	REDIS_CATCH;
	return 0;
}

int RedisClient::lrem(const std::string& key, const std::string& value)
{
	REDIS_TRY
	{
		return (*this)->lrem(key, 0, value);
	}
	REDIS_CATCH;
	return 0;
}
