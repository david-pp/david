#ifndef __HTTP_CLIENT_H_
#define __HTTP_CLIENT_H_

#include <map>
#include <curl/curl.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

typedef std::string HttpURL;
typedef std::string HttpResult;

class HttpWork;

class HttpConnection
{
public:
	HttpConnection();
	~HttpConnection();

	const HttpResult& get(const HttpURL& url, long timeout_ms = 0);

	bool isOkay() { return result_okay; }

	void setVerbose();
	void setTimeout(long ms);
	void setConnectionTimeout(long ms);
	void setURL(const HttpURL& url);

	void setWork(HttpWork* w);

public:
	bool init();
	bool fini();
	void onRecv(const char* content, size_t size);

	CURL* easy;
	std::string url;
	char error[CURL_ERROR_SIZE];
	HttpResult result;
	bool result_okay;

	HttpWork* work;
};

struct HttpCallBack
{
	virtual void callback(HttpConnection* http)
	{
	}
};

class HttpWork
{
public:
	HttpWork();
	~HttpWork();

	bool init();
	void fini();
	void run();
	void timer();

	bool get(const HttpURL& url, HttpCallBack* callback, long timeout_ms = 0);

public:
	int curlmTimerFunction(long timeout_ms);
	int curlmSocketFunction(CURL* easy, curl_socket_t s, int what, void *sockp);

	void onHttpTimer(const boost::system::error_code& error);

	curl_socket_t onHttpOpenSocket();

	void onHttpCloseSocket(curl_socket_t socket);

	bool new_conn(const char *url);

	void event_cb(boost::asio::ip::tcp::socket * tcp_socket, int action);
	void remsock(int *f);
	void setsock(int *fdp, curl_socket_t s, CURL* e, int act);
	void addsock(curl_socket_t s, CURL *easy, int action);
	void check_multi_info();


	struct ConnInfo
	{
		ConnInfo()
		{
			easy = NULL;
			bzero(error, sizeof(error));
		}

		CURL* easy;
		std::string url;
		char error[CURL_ERROR_SIZE];
	};

	boost::asio::io_service&  getIOService() { return ios_; }

private:
	typedef std::map<curl_socket_t, boost::asio::ip::tcp::socket*> SocketMap;

	boost::asio::io_service ios_;
	boost::asio::deadline_timer timer_;
	SocketMap sockets_;

	CURLM* multi_;
	int still_running_;
};


class AsyncHttpClient 
{
public:
	static AsyncHttpClient& instance();

	bool init();
	bool fini();
	int  run();

public:
	bool get(const std::string& url, HttpCallBack* cb);
	bool post(const std::string& url);

private:

};


#endif // __HTTP_CLIENT_H_