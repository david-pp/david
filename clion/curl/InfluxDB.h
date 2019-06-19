//
// Created by wangdawei on 2019/6/19.
//

#ifndef CLION_INFLUXDB_H
#define CLION_INFLUXDB_H

#include <string>
#include <chrono>
#include <deque>
#include <memory>

namespace influxdb {

    class Metric;

    class Transport;

    class InfluxDB {
    public:
        /// Disable copy constructor
        InfluxDB &operator=(const InfluxDB &) = delete;

        /// Disable copy constructor
        InfluxDB(const InfluxDB &) = delete;

        /// Constructor required valid transport
        InfluxDB(const std::string &url);

        /// Flushes buffer
        ~InfluxDB();

        /// Writes a metric
        /// \param metric
        void write(Metric &&metric);

        void write_async(Metric &&metric);

        /// Flushes metric buffer (this can also happens when buffer is full)
        void flushBuffer();

        /// Enables metric buffering
        /// \param size
        void enableBuffering(const std::size_t size = 32);

        /// Adds a global tag
        /// \param name
        /// \param value
        void addGlobalTag(const std::string &name, const std::string &value);

        void addGlobalFieldInt(const std::string &name, int value);

        void addGlobalFieldFloat(const std::string &name, double value);

        void addGlobalFieldString(const std::string &name, const std::string &value);

        /// Converts point to Influx Line Protocol
        std::string toLineProtocol(const Metric& metric);

        std::string toLineProtocolWithTimestamp(const Metric& metric);

    private:
        /// Buffer for points
        std::deque<std::string> mBuffer;

        /// Flag stating whether point buffering is enabled
        bool mBuffering;

        /// Buffer size
        std::size_t mBufferSize;

        /// Underlying transport UDP/HTTP/Unix socket
        std::shared_ptr<Transport> mTransport;

        /// Transmits string over transport
        void transmit(std::string &&point);

        /// List of global tags
        std::string mGlobalTags;

        /// List of global fields
        std::string mGlobalFields;
    };


/// \brief Represents a point
    class Metric {
    public:
        friend class InfluxDB;

        /// Constructs point based on measurement name
        Metric(const std::string &measurement);

        /// Default destructor
        ~Metric() = default;

        /// Adds a tags
        Metric &&tag(const std::string &key, const std::string &value);

        /// Adds filed
        Metric &&fieldInt(const std::string &name, long value);

        Metric &&fieldFloat(const std::string &name, double value);

        Metric &&fieldString(const std::string &name, const std::string &value);

        /// Generetes current timestamp
        static auto getCurrentTimestamp() -> decltype(std::chrono::system_clock::now());

        /// Converts point to Influx Line Protocol
        std::string toLineProtocol();

        std::string toLineProtocolWithTimestamp();

    protected:
        /// A name
        std::string mMeasurement;

        /// A timestamp
        std::chrono::time_point<std::chrono::system_clock> mTimestamp;

        /// Tags
        std::string mTags;

        /// Fields
        std::string mFields;
    };

    /// \brief Transport interface
    class Transport {
    public:
        Transport() = default;

        virtual ~Transport() = default;

        /// Sends string blob
        virtual void send(std::string &&message) = 0;

        /// Sends s request
        virtual void query(const std::string & /*query*/) {
            throw std::runtime_error("Queries are not supported in the selected transport");
        }
    };

} // namespace influxdb

#endif //CLION_INFLUXDB_H
