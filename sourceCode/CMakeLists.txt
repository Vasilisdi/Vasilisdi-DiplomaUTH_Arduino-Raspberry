cmake_minimum_required(VERSION 3.15)
project(VasilisdiProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(PahoMqttCpp REQUIRED)

add_executable(main
    main.cpp
    mqtt_publish.cpp
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(main
    Boost::boost
    yaml-cpp::yaml-cpp
    PahoMqttCpp::paho-mqttpp3
)
