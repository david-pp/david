//
// Created by wangdawei on 2018/3/14.
//

#include "tinyhttp.h"


TinyHttp::TinyHttp() {
    RestClient::init();

    thread_ = std::thread(&TinyHttp::run_backgroud, this);
}

TinyHttp::~TinyHttp() {
    over_ = true;
    thread_.join();
    RestClient::disable();
}

void TinyHttp::run_backgroud() {
    while (!over_) {

        AsyncTaskPtr task;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex_);
            if (tasks_.size() > 0) {
                task = tasks_.front();
                tasks_.pop_front();
            }
        }

        if (task)
            task->request();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void TinyHttp::async(const RequestFunc &request) {
    AsyncTaskPtr task = std::make_shared<AsyncTask>();
    if (task) {
        task->request = request;
        std::lock_guard<std::mutex> lock(tasks_mutex_);
        tasks_.push_back(task);
    }
}

void TinyHttp::async_get(const std::string &url, const ResponseCallback &cb) {
    async([url, cb]() {
        Response resp = RestClient::get(url);
        if (cb)
            cb(resp);
    });
}

void TinyHttp::async_post(const std::string &url, const std::string &ctype, const std::string &data,
                          const ResponseCallback &cb) {
    async([url, ctype, data, cb]() {
        Response resp = RestClient::post(url, ctype, data);
        if (cb)
            cb(resp);
    });
}
