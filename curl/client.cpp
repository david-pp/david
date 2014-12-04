
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

	const std::string& result = http.get("http://www.example.com/", 5000);

	std::cout << "-------------- " << std::endl;
	std::cout << result << std::endl;
	std::cout << "error:" << http.error << std::endl;
	//std::cout << http.get("www.baidu.com") << std::endl;
}

int main(int argc, const char* argv [])
{
	test_boost();
	test_HttpConnection();
	return 0;

	HttpWork http;
	if (http.init())
	{
		for (size_t i = 0; i < 1000; ++i)
			http.new_conn("http://www.baidu.com/");
		http.run();
	}
#if 0
	// ..
	{
		HttpWork http;
		if (http.init())
		{
			http.run();
		}

	}

#endif


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