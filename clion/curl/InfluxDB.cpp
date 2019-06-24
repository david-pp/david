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

namespace influxdb {

    void httpPost(const std::string& url, const std::string& data) {
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
            throw std::runtime_error(curl_easy_strerror(response));
        }
        if (responseCode < 200 || responseCode > 206) {
//                std::cout <<  std::to_string(responseCode) << std::endl;
            throw std::runtime_error("Response code : " + std::to_string(responseCode));
        }

        curl_easy_cleanup(curl);
    }

    namespace transports {

        /// \brief HTTP transport
        class HTTP : public Transport {
        public:
            /// Constructor
            HTTP(const std::string &url);

            /// Default destructor
            ~HTTP() = default;

            /// Sends point via HTTP POST
            void send(std::string &&post);

            /// Enable Basic Auth
            /// \param auth <username>:<password>
            void enableBasicAuth(const std::string &auth);

            /// Enable SSL
            void enableSsl();

        private:
            /// Custom deleter of CURL object
            static void deleteCurl(CURL *curl);

            /// Initilizes CURL and all common options
            CURL *initCurl(const std::string &url);

            /// CURL smart pointer with custom deleter
            std::unique_ptr<CURL, decltype(&HTTP::deleteCurl)> curlHandle;
        };

        HTTP::HTTP(const std::string &url) :
                curlHandle(initCurl(url), &HTTP::deleteCurl) {
        }

        CURL *HTTP::initCurl(const std::string &url) {
            CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_ALL);
            if (globalInitResult != CURLE_OK) {
                throw std::runtime_error(std::string("cURL init") + curl_easy_strerror(globalInitResult));
            }

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
            return curl;
        }

        void HTTP::enableBasicAuth(const std::string &auth) {
            CURL *curl = curlHandle.get();
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
        }

        void HTTP::enableSsl() {
            CURL *curl = curlHandle.get();
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        }

        void HTTP::deleteCurl(CURL *curl) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
        }

        void HTTP::send(std::string &&post) {
            CURLcode response;
            long responseCode;
            curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, post.c_str());
            curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDSIZE, (long) post.length());
            response = curl_easy_perform(curlHandle.get());
            curl_easy_getinfo(curlHandle.get(), CURLINFO_RESPONSE_CODE, &responseCode);
            if (response != CURLE_OK) {
                throw std::runtime_error(curl_easy_strerror(response));
            }
            if (responseCode < 200 || responseCode > 206) {
//                std::cout <<  std::to_string(responseCode) << std::endl;
                throw std::runtime_error("Response code : " + std::to_string(responseCode));
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////

    InfluxDB::InfluxDB(const std::string &url) :
            mTransport(new transports::HTTP(url)), mURL(url) {
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
            transmit(std::move(point));
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

    void InfluxDB::transmit(std::string &&point) {
        httpPost(mURL, point);
//        mTransport->send(std::move(point));
    }

    void InfluxDB::write(Metric &&metric) {
        if (mBuffering) {
            mBuffer.emplace_back(toLineProtocol(metric));
        } else {
            transmit(toLineProtocol(metric));
        }
    }

    void InfluxDB::write_async(Metric &&metric) {
        std::cout << toLineProtocol(metric) << std::endl;

    }

    std::string InfluxDB::toLineProtocol(const Metric &metric) {
        return metric.mMeasurement + mGlobalTags + metric.mTags + " " + metric.mFields + mGlobalFields;
    }

    std::string InfluxDB::toLineProtocolWithTimestamp(const Metric &metric) {
        return std::string();
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

    auto Metric::getCurrentTimestamp() -> decltype(std::chrono::system_clock::now()) {
        return std::chrono::system_clock::now();
    }

    std::string Metric::toLineProtocol() {
        return mMeasurement + mTags + " " + mFields;
    }

    std::string Metric::toLineProtocolWithTimestamp() {
        return mMeasurement + mTags + " " + mFields + " " + std::to_string(
                std::chrono::duration_cast<std::chrono::nanoseconds>(mTimestamp.time_since_epoch()).count()
        );
    }

    //////////////////////////////////////////////////////////////////////////////

} // namespace influxdb
