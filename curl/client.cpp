
#include <iostream>
#include <unistd.h>
#include "HttpClient.h"
#include <boost/bind.hpp>

int f(int a, int b)
{
    return a + b;
}

int g(int a, int b, int c)
{
    return a + b + c;
}

void test_boost()
{
	//std::cout << boost::bind(f, _1, _2)(6, 5) << std::endl;
}

void test_HttpConnection()
{
	HttpConnection http;

	const std::string& result = http.get("http://www.example.com/", 1000);
	if (http.isOkay())
	{
		std::cout << result << std::endl;
	}
	else
	{
		std::cout << http.error << std::endl;
	}

	std::cout << http.get("www.linuxvirtualserveraa.org") << std::endl;
}

void test_HttpWork()
{
	HttpWork http;

	struct MyCB : public HttpCallBack
	{
		virtual void callback(HttpConnection* http)
		{
			std::cout << http->result << std::endl;
		}
	};

	for (size_t i = 0; i < 1; ++i)
		http.get("http://www.baidu.com/", new MyCB);

	http.run();
}

int main(int argc, const char* argv [])
{
	//test_boost();
	//test_HttpConnection();
	test_HttpWork();
	return 0;

	AsyncHttpClient::instance().init();

	while (true)
	{
		std::cout << "----------- timer ------------" << std::endl;
		AsyncHttpClient::instance().run();
		usleep(1000000);
	}

	AsyncHttpClient::instance().fini();
}