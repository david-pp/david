cmake_minimum_required(VERSION 3.2)
project(clion)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(timeusage timeusage.cpp)
target_link_libraries(timeusage curl)

add_executable(example_influxdb example_influxdb.cpp InfluxDB.cpp InfluxDB.h)
target_link_libraries(example_influxdb curl)