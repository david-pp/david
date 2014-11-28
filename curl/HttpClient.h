#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include <map>
#include <curl/curl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

class HttpWork
{
	typedef std::map<curl_socket_t, boost::asio::ip::tcp::socket *> SocketMap;

	boost::asio::io_service ios_;
	boost::asio::deadline_timer timer_;
	SocketMap sockets_;
};

class AsyncHttpClient 
{
public:
	static AsyncHttpClient& instance();

	bool init();
	bool fini();
	int  run();

public:
	bool get(const std::string& url);
	bool post(const std::string& url);

private:

};


#endif // __HTTP_CLIENT_H_