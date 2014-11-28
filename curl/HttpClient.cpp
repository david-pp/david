#include "HttpClient.h"

AsyncHttpClient& AsyncHttpClient::instance()
{
	static AsyncHttpClient client;
	return client;
}

bool AsyncHttpClient::init()
{
	return true;
}

bool AsyncHttpClient::fini()
{
	return true;
}

int  AsyncHttpClient::run()
{
	return 0;
}