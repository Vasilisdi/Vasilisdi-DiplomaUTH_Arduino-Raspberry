#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <yaml-cpp/yaml.h>
#include "../include/mqtt_publish.h"

const std::string port_option = "port_windows";
//const std::string port_option = "port_linux";

struct SerialConfig {
    std::string port;
    unsigned int baud_rate;
};

SerialConfig loadSerialConfig() {
    YAML::Node config = YAML::LoadFile("config.yaml");
    SerialConfig serialConfig;

    serialConfig.port = config["serial"][port_option].as<std::string>();

    serialConfig.baud_rate = config["serial"]["baud_rate"].as<unsigned int>();
    return serialConfig;
}


std::vector<float> parseLine(const std::string& line) {
    std::vector<float> values;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try {
            values.push_back(std::stof(token));
        } catch (...) {
            // skip invalid entries
        }
    }
    return values;
}

int main() {
    auto serialConfig = loadSerialConfig();
    std::string portName = serialConfig.port;
    unsigned int baudRate = serialConfig.baud_rate;

    std::cout << "Using port: " << portName << ", baud: " << baudRate << std::endl;

    boost::asio::io_context io;
    boost::asio::serial_port serial(io, portName);
    serial.set_option(boost::asio::serial_port_base::baud_rate(baudRate));

    MQTTClient mqttClient("tcp://localhost:1883", "edge_collector");
    mqttClient.connect();

    std::cout << "Starting serial read + MQTT publish..." << std::endl;

    while (true) {
        std::string line;
        char c;
        try {
            while (true) {
                boost::asio::read(serial, boost::asio::buffer(&c, 1));
                if (c == '\n') break;
                line += c;
            }

            auto values = parseLine(line);
            if (values.size() == 3) {
                auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();

                std::string json = "{ \"timestamp\": " + std::to_string(timestamp) +
                                   ", \"ax\": " + std::to_string(values[0]) +
                                   ", \"ay\": " + std::to_string(values[1]) +
                                   ", \"az\": " + std::to_string(values[2]) + " }";

                mqttClient.publish("vibration/sensor1", json);
                std::cout << "Published: " << json << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << ", line: " << line << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    mqttClient.disconnect();

    return 0;
}