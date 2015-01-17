#include "HttpClient.h"
#include <boost/bind.hpp>

#define MSG_OUT stdout
#define TRACE(log) std::cout << __PRETTY_FUNCTION__ << ":" << this << ":" << log << std::endl;
#define DEBUG std::cout << __PRETTY_FUNCTION__ << ":" << this << ":"


///////////////////////

static size_t httpWriteMemoryCallback(void *contents, size_t size, size_t nmemb, HttpConnection* http)
{
	size_t realsize = size * nmemb;
	if (http)
	{
		http->onRecv((const char*)contents, realsize);
	}
	
	return realsize;
}

// CURLOPT_OPENSOCKETFUNCTION
static curl_socket_t curlOpenSocketFunction(HttpWork* work, curlsocktype purpose, struct curl_sockaddr *address)
{
	// restrict to ipv4
	if (work && purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
	{
		return work->onHttpOpenSocket();
	}

	return CURL_SOCKET_BAD;
}

// CURLOPT_CLOSESOCKETFUNCTION
static int curlCloseSocketFunction(HttpWork* work, curl_socket_t item)
{
	if (work) work->onHttpCloseSocket(item);
	return 0;
}

HttpConnection::HttpConnection()
{
	result_okay = false;
	easy = NULL;
	memset(error, 0, sizeof(error));
	init();
}

HttpConnection::~HttpConnection()
{
	fini();
}

bool HttpConnection::init()
{
	easy = curl_easy_init();
	if (!easy)
	{
		std::cerr << "curl_easy_init() failed, exiting!" << std::endl;
		return false;
	}

	curl_easy_setopt(this->easy, CURLOPT_WRITEFUNCTION, httpWriteMemoryCallback);
	curl_easy_setopt(this->easy, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(this->easy, CURLOPT_ERRORBUFFER, this->error);
	curl_easy_setopt(this->easy, CURLOPT_PRIVATE, this);
	curl_easy_setopt(this->easy, CURLOPT_FORBID_REUSE, 1L);
	return true;
}

bool HttpConnection::fini()
{
	if (easy)
		curl_easy_cleanup(easy);
	return true;
}

void HttpConnection::setVerbose()
{
	if (easy) curl_easy_setopt(easy, CURLOPT_VERBOSE, 1L);
}

void HttpConnection::setTimeout(long ms)
{
	if (easy) curl_easy_setopt(easy, CURLOPT_TIMEOUT_MS, ms);
}

void HttpConnection::setConnectionTimeout(long ms)
{
	if (easy) curl_easy_setopt(this->easy, CURLOPT_CONNECTTIMEOUT_MS, ms);
}

void HttpConnection::setURL(const HttpURL& _url)
{
	if (easy)
	{
		this->url = _url;
		curl_easy_setopt(this->easy, CURLOPT_URL, url.c_str());
	}
}

void HttpConnection::setWork(HttpWork* w)
{
	if (w)
	{
		this->work = w;
		curl_easy_setopt(this->easy, CURLOPT_OPENSOCKETFUNCTION, ::curlOpenSocketFunction);
		curl_easy_setopt(this->easy, CURLOPT_OPENSOCKETDATA, w);

		curl_easy_setopt(this->easy, CURLOPT_CLOSESOCKETFUNCTION, ::curlCloseSocketFunction);
		curl_easy_setopt(this->easy, CURLOPT_CLOSESOCKETDATA, w);	
	}
}

void HttpConnection::onRecv(const char* content, size_t size)
{
	TRACE(size);
	result.append(content, size);
}

const HttpResult& HttpConnection::get(const HttpURL& _url, long timeout_ms)
{
	result_okay = false;
	memset(error, 0, sizeof(error));
	result.clear();

	if (easy)
	{
		setURL(_url);
		setTimeout(timeout_ms);

		CURLcode res = curl_easy_perform(easy);
		if(res != CURLE_OK) 
		{
			result_okay = false;
			strncpy(error, curl_easy_strerror(res), CURL_ERROR_SIZE - 1);
		}
		else
		{
			result_okay = true;
		}
	}

	return result;
}

//////////


// CURLMOPT_TIMERFUNCTION : Update the event timer after curl_multi library calls
static int curlmTimerFunction(CURLM *multi, long timeout_ms, HttpWork* work)
{
	return work ? work->curlmTimerFunction(timeout_ms) : 0;
}

// CURLMOPT_SOCKETFUNCTION
static int curlmSocketFunction(CURL *e, curl_socket_t s, int what, HttpWork* work, void *sockp)
{
	return work ? work->curlmSocketFunction(e, s, what, sockp) : 0;
}

void HttpWork::check_multi_info()
{
  char *eff_url;
  CURLMsg *msg;
  int msgs_left;
  HttpConnection *conn;
  CURL *easy;
  CURLcode res;

  fprintf(MSG_OUT, "\nREMAINING: %d", still_running_);

  while ((msg = curl_multi_info_read(multi_, &msgs_left)))
  {
    if (msg->msg == CURLMSG_DONE)
    {
      easy = msg->easy_handle;
      res = msg->data.result;
      curl_easy_getinfo(easy, CURLINFO_PRIVATE, &conn);
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
      fprintf(MSG_OUT, "\nDONE: %s => (%d) %s", eff_url, res, conn->error);
      curl_multi_remove_handle(multi_, easy);
      curl_easy_cleanup(easy);
      //free(conn);
    }
  }
}


void HttpWork::event_cb(boost::asio::ip::tcp::socket * tcp_socket, int action)
{
  fprintf(MSG_OUT, "\nevent_cb: action=%d", action);

  CURLMcode rc;
  rc = curl_multi_socket_action(multi_, tcp_socket->native_handle(), action, &still_running_);

  //mcode_or_die("event_cb: curl_multi_socket_action", rc);
  check_multi_info();

  if (still_running_ <= 0 )
  {
    fprintf(MSG_OUT, "\nlast transfer done, kill timeout");
    timer_.cancel();
  }
}

/* Clean up any data */
void HttpWork::remsock(int *f)
{
  fprintf(MSG_OUT, "\nremsock: %d", *f);

  if ( f )
  {
    free(f);
  }
}

void HttpWork::setsock(int *fdp, curl_socket_t s, CURL* e, int act)
{
  fprintf(MSG_OUT, "\nsetsock: socket=%d, act=%d, fdp=%p", s, act, fdp);

  SocketMap::iterator it = sockets_.find(s);

  if (it == sockets_.end())
  {
    fprintf(MSG_OUT, "\nsocket %d is a c-ares socket, ignoring", s);
    return;
  }

  boost::asio::ip::tcp::socket * tcp_socket = it->second;

  *fdp = act;

  if ( act == CURL_POLL_IN )
  {
    fprintf(MSG_OUT, "\nwatching for socket to become readable");

    tcp_socket->async_read_some(boost::asio::null_buffers(),
                             boost::bind(&HttpWork::event_cb, this,
                               tcp_socket,
                               act));
  }
  else if ( act == CURL_POLL_OUT )
  {
    fprintf(MSG_OUT, "\nwatching for socket to become writable");

    tcp_socket->async_write_some(boost::asio::null_buffers(),
                              boost::bind(&HttpWork::event_cb, this,
                                tcp_socket,
                                act));
  }
  else if ( act == CURL_POLL_INOUT )
  {
    fprintf(MSG_OUT, "\nwatching for socket to become readable & writable");

    tcp_socket->async_read_some(boost::asio::null_buffers(),
                             boost::bind(&HttpWork::event_cb, this,
                               tcp_socket,
                               act));

    tcp_socket->async_write_some(boost::asio::null_buffers(),
                              boost::bind(&HttpWork::event_cb, this,
                                tcp_socket,
                                act));
  }
}


void HttpWork::addsock(curl_socket_t s, CURL *easy, int action)
{
  int *fdp = (int *)calloc(sizeof(int), 1); /* fdp is used to store current action */

  this->setsock(fdp, s, easy, action);
  curl_multi_assign(multi_, s, fdp);
}

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t written = size * nmemb;

  char* pBuffer = (char*)malloc(written + 1);

  strncpy(pBuffer, (const char *)ptr, written);
  pBuffer [written] = '\0';

  fprintf(MSG_OUT, "%s", pBuffer);

  free(pBuffer);

  return written;
}

/* CURLOPT_OPENSOCKETFUNCTION */
static curl_socket_t opensocket(void *clientp, curlsocktype purpose, struct curl_sockaddr *address)
{
	HttpWork* work = (HttpWork*)clientp;

	/* restrict to ipv4 */
	if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
	{
		return work->onHttpOpenSocket();
	}

	return CURL_SOCKET_BAD;
}

/* CURLOPT_CLOSESOCKETFUNCTION */
static int closesocket(void *clientp, curl_socket_t item)
{
	HttpWork* work = (HttpWork*)clientp;
	work->onHttpCloseSocket(item);
	return 0;
}



HttpWork::HttpWork() : timer_(ios_)
{
	multi_ = NULL;
	init();
}

HttpWork::~HttpWork()
{
	fini();
}

bool HttpWork::init()
{
	TRACE("");

	multi_ = curl_multi_init();
	if (!multi_)
	{
		std::cerr << "error:curl_multi_init() failed !" << std::endl;
		return false;
	}

	curl_multi_setopt(multi_, CURLMOPT_SOCKETFUNCTION, ::curlmSocketFunction);
	curl_multi_setopt(multi_, CURLMOPT_SOCKETDATA, this);
	curl_multi_setopt(multi_, CURLMOPT_TIMERFUNCTION, ::curlmTimerFunction);
	curl_multi_setopt(multi_, CURLMOPT_TIMERDATA, this);

	return true;
}

void HttpWork::fini()
{
	TRACE("");

	if (multi_)
		curl_multi_cleanup(multi_);
}

void HttpWork::timer()
{
	std::cout << this << ": Hello, world!\n";

	//timer_.expires_from_now(boost::posix_time::milliseconds(1000));
	//timer_.async_wait(boost::bind(&HttpWork::timer, this));
}

void HttpWork::run()
{
	TRACE("begin");
	//timer_.expires_from_now(boost::posix_time::milliseconds(1000));
	//timer_.async_wait(boost::bind(&HttpWork::timer, this));

	ios_.run();

	TRACE("done");
}



void HttpWork::onHttpTimer(const boost::system::error_code& error)
{
	TRACE(error);

	if (!error)
	{
		CURLMcode rc = curl_multi_socket_action(multi_, CURL_SOCKET_TIMEOUT, 0, &still_running_);
		if (rc != CURLM_OK)
		{
			std::cerr << "curl_multi_socket_action : " << curl_multi_strerror(rc) << std::endl;
			return;
		}
		check_multi_info();
	}
}

int HttpWork::curlmTimerFunction(long timeout_ms)
{
	TRACE(timeout_ms);

	timer_.cancel();

	if (timeout_ms > 0)
	{
		timer_.expires_from_now(boost::posix_time::millisec(timeout_ms));
		timer_.async_wait(boost::bind(&HttpWork::onHttpTimer, this, _1));
	}
	else
	{
		boost::system::error_code error;
		this->onHttpTimer(error);
	}

	return 0;
}

int HttpWork::curlmSocketFunction(CURL* e, curl_socket_t s, int what, void *sockp)
{
	fprintf(MSG_OUT, "\nsock_cb: socket=%d, what=%d, sockp=%p", s, what, sockp);

	HttpWork* work = this;
	int *actionp = (int*) sockp;

	const char *whatstr[]={ "none", "IN", "OUT", "INOUT", "REMOVE"};

	fprintf(MSG_OUT,
	      "\nsocket callback: s=%d e=%p what=%s ", s, e, whatstr[what]);

	if (what == CURL_POLL_REMOVE )
	{
		//fprintf(MSG_OUT, "\n");
		work->remsock(actionp);
	}
	else
	{
		if (!actionp)
		{
		  //fprintf(MSG_OUT, "\nAdding data: %s", whatstr[what]);
		  work->addsock(s, e, what);
		}
		else
		{
		  //fprintf(MSG_OUT, "\nChanging action from %s to %s", whatstr[*actionp], whatstr[what]);
		  work->setsock(actionp, s, e, what);
		}
	}

	return 0;	
}

curl_socket_t HttpWork::onHttpOpenSocket()
{
	curl_socket_t sockfd = CURL_SOCKET_BAD;

	/* create a tcp socket object */
	boost::asio::ip::tcp::socket *tcp_socket = new boost::asio::ip::tcp::socket(ios_);

	/* open it and get the native handle*/
	boost::system::error_code ec;
	tcp_socket->open(boost::asio::ip::tcp::v4(), ec);

	if (ec)
	{
		//An error occurred
		std::cout << std::endl << "Couldn't open socket [" << ec << "][" << ec.message() << "]";
	}
	else
	{
		sockfd = tcp_socket->native_handle();

		/* save it for monitoring */
		sockets_.insert(std::pair<curl_socket_t, boost::asio::ip::tcp::socket *>(sockfd, tcp_socket));

		TRACE(sockfd);
	}

	return sockfd;
}

void HttpWork::onHttpCloseSocket(curl_socket_t socket)
{
	SocketMap::iterator it = sockets_.find(socket);
	if (it != sockets_.end())
	{
		delete it->second;
		sockets_.erase(it);
	}
}


bool HttpWork::get(const HttpURL& url, HttpCallBack* callback, long timeout_ms)
{
	if (!callback) return false;

	HttpConnection* conn = new HttpConnection;
	if (!conn) return false;

	conn->setURL(url);
	conn->setTimeout(timeout_ms);
	conn->setWork(this);

	CURLMcode rc = curl_multi_add_handle(multi_, conn->easy);
    if (rc != CURLM_OK)
    {
    	delete callback;
    	delete conn;
    	std::cerr << "curl_multi_add_handle() failed:" << curl_multi_strerror(rc) << std::endl;
    	return false;
    }

	return true;
}


/* Create a new easy handle, and add it to the global curl_multi */
bool HttpWork::new_conn(const char* url)
{
	TRACE("begin...");
	ConnInfo* conn = new ConnInfo;
	if (!conn)
	{
		return false;
	}

	CURL* easy = curl_easy_init();
	if (!easy)
	{
		delete conn;
		std::cout << "curl_easy_init() failed, exiting!" << std::endl;
		return false;
	}

	conn->easy = easy;
	conn->url = url;

	curl_easy_setopt(conn->easy, CURLOPT_URL, url);
	curl_easy_setopt(conn->easy, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(conn->easy, CURLOPT_WRITEDATA, &conn);
	//curl_easy_setopt(conn->easy, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(conn->easy, CURLOPT_ERRORBUFFER, conn->error);
	curl_easy_setopt(conn->easy, CURLOPT_PRIVATE, conn);
	//curl_easy_setopt(conn->easy, CURLOPT_NOPROGRESS, 1L);
	//curl_easy_setopt(conn->easy, CURLOPT_PROGRESSFUNCTION, prog_cb);
	//curl_easy_setopt(conn->easy, CURLOPT_PROGRESSDATA, conn);
	//curl_easy_setopt(conn->easy, CURLOPT_LOW_SPEED_TIME, 3L);
	//curl_easy_setopt(conn->easy, CURLOPT_LOW_SPEED_LIMIT, 10L);

	/* call this function to get a socket */
	curl_easy_setopt(conn->easy, CURLOPT_OPENSOCKETFUNCTION, opensocket);
	curl_easy_setopt(conn->easy, CURLOPT_OPENSOCKETDATA, this);

	/* call this function to close a socket */
	curl_easy_setopt(conn->easy, CURLOPT_CLOSESOCKETFUNCTION, closesocket);
	curl_easy_setopt(conn->easy, CURLOPT_CLOSESOCKETDATA, this);
	curl_easy_setopt(conn->easy, CURLOPT_FORBID_REUSE, 1L);
	curl_easy_setopt(conn->easy, CURLOPT_TIMEOUT_MS, 5000L);
	curl_easy_setopt(conn->easy, CURLOPT_CONNECTTIMEOUT_MS, 1000L);

    CURLMcode rc = curl_multi_add_handle(multi_, conn->easy);
    if (rc != CURLM_OK)
    {
    	std::cout << "curl_multi_add_handle() failed:" << curl_multi_strerror(rc) << std::endl;

    	return false;
    }

    TRACE("curl_multi_add_handle .. ok");
    return true;
}

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