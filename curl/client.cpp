
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

int main(int argc, const char* argv [])
{
	test_boost();

	HttpWork http;
	if (http.init())
	{
		for (size_t i = 0; i < 1; ++i)
			http.new_conn("http://www.example.com/");
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