//
// Created by wangdawei on 2019/6/19.
//

#include "InfluxDB.h"
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <functional>
#include <queue>
#include <atomic>
#include <condition_variable>

//#define DEBUG_INFLUXDB

namespace influxdb {

    ////////////////////////////////////////////////////////////////////////////

    static void httpPost(const std::string &url, const std::string &data) {

#ifdef DEBUG_INFLUXDB
        std::cout << url << ":" << data << std::endl;
#endif

        CURL *curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
        FILE *devnull = fopen("/dev/null", "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, devnull);

        CURLcode response;
        long responseCode;
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long) data.length());
        response = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
        if (response != CURLE_OK) {
            //    throw std::runtime_error(curl_easy_strerror(response));
        }
        if (responseCode < 200 || responseCode > 206) {
            //    std::cout <<  std::to_string(responseCode) << std::endl;
            // throw std::runtime_error("Response code : " + std::to_string(responseCode));
        }

        curl_easy_cleanup(curl);
    }

    ////////////////////////////////////////////////////////////////////////////

    class AsyncTaskManager {
    public:
        typedef std::function<void()> RequestFunc;

        struct AsyncTask {
            RequestFunc request;
        };

        typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;

    public:
        AsyncTaskManager() {
            thread_ = std::thread(&AsyncTaskManager::run_backgroud, this);
        }

        ~AsyncTaskManager() {
            over_ = true;
            tasks_cond_.notify_all();
            thread_.join();
        }

        void async(const RequestFunc &request) {
            AsyncTaskPtr task = std::make_shared<AsyncTask>();
            if (task) {
                task->request = request;
                std::lock_guard<std::mutex> lock(tasks_mutex_);
                tasks_.emplace_back(task);
            }

            tasks_cond_.notify_all();
        }

    protected:
        void run_backgroud() {
            while (!over_) {

                AsyncTaskPtr task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mutex_);
                    tasks_cond_.wait(lock, [this] { return !tasks_.empty() || over_; });

                    if (tasks_.size() > 0) {
                        task = tasks_.front();
                        tasks_.pop_front();
                    }
                }

                if (task)
                    task->request();
            }
        }

        std::deque<AsyncTaskPtr> tasks_;
        std::mutex tasks_mutex_;
        std::condition_variable tasks_cond_;

    private:
        std::thread thread_;
        std::atomic_bool over_;
    };

    ///////////////////////////////////////////////////////////////////////////////

    InfluxDB::InfluxDB(const std::string &url) : mURL(url) {
    }

    std::shared_ptr<AsyncTaskManager> InfluxDB::getAsyncMgr() {
        if (!async_) {
            async_ = std::make_shared<AsyncTaskManager>();
        }

        return async_;
    }

    void InfluxDB::enableBuffering(const std::size_t size) {
        mBufferSize = size;
        mBuffering = true;
    }

    void InfluxDB::flushBuffer() {
        if (!mBuffering) {
            return;
        }
        for (auto &&point : mBuffer) {
            httpPost(mURL, std::move(point));
        }
    }

    void InfluxDB::addGlobalTag(const std::string &key, const std::string &value) {
        if (key.empty() || value.empty())
            return;

        mGlobalTags += ",";
        mGlobalTags += key;
        mGlobalTags += "=";
        mGlobalTags += value;
    }

    void InfluxDB::addGlobalFieldInt(const std::string &name, int value) {
        if (name.empty()) return;

        mGlobalFields += ",";
        mGlobalFields += name;
        mGlobalFields += "=";
        mGlobalFields += std::to_string(value);
        mGlobalFields += "i";
    }

    void InfluxDB::addGlobalFieldFloat(const std::string &name, double value) {
        if (name.empty()) return;

        mGlobalFields += ",";
        mGlobalFields += name;
        mGlobalFields += "=";
        mGlobalFields += std::to_string(value);
    }

    void InfluxDB::addGlobalFieldString(const std::string &name, const std::string &value) {
        if (name.empty()) return;

        if (!mGlobalFields.empty()) mGlobalFields += ",";
        mGlobalFields += name;
        mGlobalFields += "=";
        mGlobalFields += "\"";
        mGlobalFields += value;
        mGlobalFields += "\"";
    }

    InfluxDB::~InfluxDB() {
        if (mBuffering) {
            flushBuffer();
        }
    }

    void InfluxDB::write(Metric &&metric) {
        if (mBuffering) {
            mBuffer.emplace_back(toLineProtocol(metric));
        } else {
            httpPost(mURL, toLineProtocol(metric));
        }
    }

    void InfluxDB::write_async(Metric &&metric) {
        std::string url = mURL;
        std::string content = toLineProtocol(metric);

        getAsyncMgr()->async([url, content]() {
            httpPost(url, content);
        });
#ifdef  DEBUG_INFLUXDB
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
    }

    std::string InfluxDB::toLineProtocol(const Metric &metric) {
        if (metric.mWithTimestamp)
            return metric.mMeasurement + mGlobalTags + metric.mTags + " " + metric.mFields + mGlobalFields + " " +
                   std::to_string(
                           std::chrono::duration_cast<std::chrono::nanoseconds>(
                                   metric.mTimestamp.time_since_epoch()).count());
        else
            return metric.mMeasurement + mGlobalTags + metric.mTags + " " + metric.mFields + mGlobalFields;
    }

    ////////////////////////////////////////////////////

    Metric::Metric(const std::string &measurement) :
            mMeasurement(measurement), mTimestamp(Metric::getCurrentTimestamp()) {
    }

    Metric &&Metric::fieldInt(const std::string &name, long value) {
        if (name.empty())
            return std::move(*this);

        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << name << "=";
        convert << value << 'i';

        mFields += convert.str();
        return std::move(*this);
    }

    Metric &&Metric::fieldFloat(const std::string &name, double value) {
        if (name.empty())
            return std::move(*this);

        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << name << "=";
        convert << value;

        mFields += convert.str();
        return std::move(*this);
    }

    Metric &&Metric::fieldString(const std::string &name, const std::string &value) {
        if (name.empty()) return std::move(*this);

        std::stringstream convert;
        if (!mFields.empty()) convert << ",";

        convert << name << "=";
        convert << '"' << value << '"';

        mFields += convert.str();
        return std::move(*this);
    }

    Metric &&Metric::tag(const std::string &key, const std::string &value) {
        if (key.empty() || value.empty())
            return std::move(*this);

        mTags += ",";
        mTags += key;
        mTags += "=";
        mTags += value;
        return std::move(*this);
    }

    Metric &&Metric::withTimestamp() {
        mWithTimestamp = true;
        return std::move(*this);
    }

    auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now()) {
        return std::chrono::system_clock::now();
    }

    std::string Metric::toLineProtocol() {
        if (mWithTimestamp)
            return mMeasurement + mTags + " " + mFields + " " + std::to_string(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count());
        else
            return mMeasurement + mTags + " " + mFields;
    }

    //////////////////////////////////////////////////////////////////////////////

} // namespace influxdb
