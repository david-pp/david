#include <iostream>
#include "ip138.h"


void test() {
	std::cout << "load: " << IpDatabase::instance().loadIp138("ip.txt") << std::endl;

	std::cout << inet_addr("222.73.33.156") << std::endl;

	IpInfo* info = IpDatabase::instance().getIpInfo("222.73.33.156");
	if (info) {
		std::cout << info->start_ip << ","
			      << info->end_ip << ","
			      << info->country << ","
			      << info->province << ","
			      << info->city << ","
			      << info->nettype << "," << std::endl;
	} else {
		std::cout << "not exist" << std::endl;
	}

	if(IpDatabase::instance().test())
		std::cout << "[test] all passed" << std::endl;
	else
		std::cout << "[test] failed" << std::endl;

	std::cout << "[test] size: " << IpDatabase::instance().size() << std::endl;
}

int getIpByList(int argc, const char* argv[]) {
	// test();
	IpDatabase::instance().loadIp138("ip.txt");

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			std::string ip = argv[i];
			auto info = IpDatabase::instance().getIpInfo(ip);
			if (info)
			{
				std::cout << ip <<  ", "  << info->asString() << std::endl;
			}
		}
	}
	return 0;
}

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " iplist-file" << std::endl; 
		return 1;
	}

	std::ifstream file(argv[1]);
	if (!file.is_open()) {
		std::cout << "[ERROR] ip file not exist: " << argv[1] << std::endl;
		return 1;
	}

	IpDatabase::instance().loadIp138("ip.txt");

	std::string ip;
	while (std::getline(file, ip)) {
		auto info = IpDatabase::instance().getIpInfo(ip);
		if (info)
		{
			std::cout << ip <<  ", " << info->asString() << std::endl;
		}
	}

	return 0;
}