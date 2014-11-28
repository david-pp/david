
#include <iostream>
#include <unistd.h>
#include "HttpClient.h"

int main(int argc, const char* argv [])
{
	AsyncHttpClient::instance().init();

	while (true)
	{
		std::cout << "----------- timer ------------" << std::endl;
		AsyncHttpClient::instance().run();
		usleep(1000000);
	}

	AsyncHttpClient::instance().fini();
}