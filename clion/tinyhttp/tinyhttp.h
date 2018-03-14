//
// Created by wangdawei on 2018/3/14.
//

#ifndef CLION_TINYHTTP_H
#define CLION_TINYHTTP_H

#include <thread>
#include <mutex>
#include <memory>
#include <functional>
#include <queue>
#include <atomic>
#include <condition_variable>

#include "restclient.h"

class TinyHttp {
public:
    typedef RestClient::Response Response;
    typedef std::function<void()> RequestFunc;
    typedef std::function<void(Response &resp)> ResponseCallback;

    struct AsyncTask {
        RequestFunc request;
    };

    typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;

public:
    TinyHttp();

    ~TinyHttp();

    void async_get(const std::string &url,
                   const ResponseCallback &cb = nullptr);

    void async_post(const std::string &url,
                    const std::string &ctype,
                    const std::string &data,
                    const ResponseCallback &cb = nullptr);

    void async(const RequestFunc &request);

protected:
    void run_backgroud();

    std::deque<AsyncTaskPtr> tasks_;
    std::mutex tasks_mutex_;

private:
    std::thread thread_;
    std::atomic_bool over_;
};

#endif //CLION_TINYHTTP_H
