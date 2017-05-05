#ifndef _IP138_H
#define _IP138_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <regex>
#include <cctype>

#include <arpa/inet.h>
#include <sys/types.h> 

struct IpInfo {
public:
	IpInfo() {
		start = 0;
		end = 0;
	}

	bool parse(const std::string& line) {
		std::vector<std::string> fields;
		std::regex re("\\,");
		std::sregex_token_iterator it(line.begin(), line.end(), re, -1);
		for (; it != std::sregex_token_iterator(); ++it) {
			fields.push_back(*it);
		}

		if (fields.size() != 8) {
			std::cerr << "[ERROR] invalid: " << line << " size:" << fields.size() << std::endl;
			return false;
		}

		start = std::atol(fields[0].c_str());
		end = std::atol(fields[1].c_str());
		start_ip = fields[2];
		end_ip = fields[3];
		country = fields[4];
		province = fields[5];
		city = fields[6];
		nettype = trimeString(fields[7]);

		// std::cout << asString() << std::endl;
		return true;
	}

	std::string asString() {
		return start_ip + "-" + end_ip + ", " + getCountry() + "," + getProvince() + "," + getCity() + "," + getNetType();
	}

	std::string trimeString(const std::string& s) {
		std::string result;
		for (size_t i = 0; i < s.size(); ++i) {
			if (!std::iscntrl(s[i]) && ! std::isspace(s[i]))
				result.push_back(s[i]);
		}
		return result;
	}

	std::string getCountry() const { return country.size() ? country : "unkown"; }
	std::string getProvince() const  { return province.size() ? province : "unkown"; }
	std::string getCity() const { return city.size() ? city : "unkown"; }
	std::string getNetType() const  { return nettype.size() ? nettype : "unkown"; }

public:
	uint32_t start;
	uint32_t end;
	std::string start_ip;
	std::string end_ip;
	std::string country;
	std::string province;
	std::string city;
	std::string nettype;
};

class IpDatabase {
public:
	static IpDatabase& instance() {
		static IpDatabase s_ip_db;
		return s_ip_db;
	}

	size_t loadIp138(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()){
			std::cerr << "[ERROR] load file failed: " << filename << std::endl;
			return 0;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line.size()) {
				IpInfo ip;
				if (ip.parse(line))
					ips_.push_back(ip);
				// std::cout << ip.asString() << std::endl;
			}
		}

		return ips_.size();
	}

	IpInfo* getIpInfo(uint32_t ipvalue) {
		if (ips_.size() == 0) return nullptr;

		size_t low = 0;
	    size_t high = ips_.size() - 1;

	    while (low <= high) {
	        size_t mid = (low + high) / 2;
	        size_t start = ips_[mid].start;
	        size_t end = ips_[mid].end;

	        if (ipvalue >= start && ipvalue <= end) 
	          return &ips_[mid];
	        else if (ipvalue < start) 
	          high = mid - 1;
	        else
	          low = mid + 1;
	    }

	    return nullptr;
	}

	IpInfo* getIpInfo(const std::string& ip) {
		return getIpInfo(ip2value(ip));
	}

	long ip2value(const std::string& ip) {
		std::vector<long> fields;
		std::regex re("\\.");
		std::sregex_token_iterator it(ip.begin(), ip.end(), re, -1);
		for (; it != std::sregex_token_iterator(); ++it) {
			std::string field = *it;
			long value = std::atol(field.c_str());
			if (value > 255 || value < 0) 
				return -1;
			fields.push_back(value);
		}

		if (fields.size() != 4)
			return false;

		return (((((fields[0]*256) + fields[1])*256)+ fields[2])*256) + fields[3];
	}

	size_t size() { return ips_.size(); }

	bool test() {
		for (auto& ip : ips_) {
			auto info = getIpInfo(ip.start_ip);
			if (!info) {
				std::cerr << "[ERROR] not exist:" << ip.start_ip << std::endl;
				return false;
			}
			if (info->start != ip.start) {
				std::cerr << "[ERROR] noe equal:" << info->start << "," << ip.start << std::endl;
				return false;
			}
			std::cout << info->asString() << std::endl;
		}
		return true;
	}


private:
	std::vector<IpInfo> ips_;
};


#endif // _IP138_H