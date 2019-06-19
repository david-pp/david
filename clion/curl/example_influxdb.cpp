//
// Created by wangdawei on 2019/6/19.
//

#include <iostream>
#include <thread>

#include "InfluxDB.h"

void test_single() {
    using namespace influxdb;

    uint32_t charid = 1024;

    InfluxDB *influx = new InfluxDB("http://127.0.0.1:8086/write?db=example");

    // ....
    influx->addGlobalTag("game", "zt2");
    influx->addGlobalTag("game_type", "17");
    influx->addGlobalTag("zone", "风雨同舟");
    influx->addGlobalTag("zone_id", "2208");

    // ....
    influx->addGlobalFieldInt("charid", charid);
    influx->addGlobalFieldString("mac", "aa-bb-cc-dd");


    // ...
    for (int i = 0; i < 10; ++i) {

        influx->write(Metric{"clientping"}
                              .tag("build", "2008")
                              .tag("version", "0.0.0")
                              .fieldInt("ping", i)
                              .fieldFloat("salary", 1024.6)
                              .fieldString("name", "david"));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void test_buffering() {

    using namespace influxdb;

    uint32_t charid = 1024;

    InfluxDB *influx = new InfluxDB("http://127.0.0.1:8086/write?db=example");

    influx->enableBuffering();

    // ....
    influx->addGlobalTag("game", "zt2");
    influx->addGlobalTag("game_type", "17");
    influx->addGlobalTag("zone", "风雨同舟");
    influx->addGlobalTag("zone_id", "2208");

    // ....
    influx->addGlobalFieldInt("charid", charid);
    influx->addGlobalFieldString("mac", "aa-bb-cc-dd");


    // ...
    for (int i = 0; i < 10; ++i) {

        influx->write(Metric{"clientping"}
                              .tag("build", "2008")
                              .tag("version", "0.0.0")
                              .fieldInt("ping", i)
                              .fieldFloat("salary", 1024.6)
                              .fieldString("name", "david"));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    influx->flushBuffer();
}

int main() {

    test_buffering();
    return 0;
}