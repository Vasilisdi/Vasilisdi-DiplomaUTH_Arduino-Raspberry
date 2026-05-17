#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <boost/asio.hpp>
#include <yaml-cpp/yaml.h>
#include <mqtt/async_client.h>
#include "mqtt/mqtt.hpp"
#include "conversion/analysis.hpp"

const std::string port_option = "port_windows";   // change to "port_linux" for Linux

struct SerialConfig {
    std::string port;
    unsigned int baud_rate;
};

struct MqttConfig {
    std::string broker;
    std::string client_id;
    std::string topic;
};

SerialConfig loadSerialConfig(const YAML::Node& config) {
    SerialConfig sc;
    sc.port = config["serial"][port_option].as<std::string>();
    sc.baud_rate = config["serial"]["baud_rate"].as<unsigned int>();
    return sc;
}

MqttConfig loadMqttConfig(const YAML::Node& config) {
    MqttConfig mc;
    mc.broker = config["mqtt"]["broker"].as<std::string>();
    mc.client_id = config["mqtt"]["client_id"].as<std::string>();
    mc.topic = config["mqtt"]["topic"].as<std::string>();
    return mc;
}

int main() {
    std::cout << "Program started" << std::endl;
    std::cout.flush();
    try {
        // Load configuration
        YAML::Node config = YAML::LoadFile("config.yaml");
        auto serialCfg = loadSerialConfig(config);
        auto mqttCfg = loadMqttConfig(config);

        std::cout << "Using port: " << serialCfg.port << ", baud: " << serialCfg.baud_rate << std::endl;
        std::cout << "MQTT broker: " << mqttCfg.broker << ", topic: " << mqttCfg.topic << std::endl;

        boost::asio::io_context io;
        boost::asio::serial_port serial(io, serialCfg.port);
        serial.set_option(boost::asio::serial_port_base::baud_rate(serialCfg.baud_rate));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

        std::cout << "Collecting data for 60 seconds...\n";

        std::vector<uint16_t> xs, ys, zs;
        auto start = std::chrono::steady_clock::now();

        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(60)) {
            uint8_t buffer[60];
            boost::system::error_code ec;
            size_t bytesRead = boost::asio::read(serial, boost::asio::buffer(buffer, 60), ec);
            if (ec) {
                std::cerr << "Read error: " << ec.message() << std::endl;
                break;
            }
            if (bytesRead == 60) {
                for (int i = 0; i < 10; ++i) {
                    uint16_t x = (buffer[i*6] << 8) | buffer[i*6+1];
                    uint16_t y = (buffer[i*6+2] << 8) | buffer[i*6+3];
                    uint16_t z = (buffer[i*6+4] << 8) | buffer[i*6+5];
                    xs.push_back(x);
                    ys.push_back(y);
                    zs.push_back(z);
                }
            } else {
                std::cerr << "Incomplete packet: " << bytesRead << " bytes\n";
            }
        }

        serial.close();
        std::cout << "\nTotal samples collected: " << xs.size() << std::endl;

        Converter converter;
        auto converted = converter.convert(xs, ys, zs);

        double sampleRate = static_cast<double>(converted.xs.size()) / 60.0; // ~15022 Hz
        FFTProcessor fft;
        auto fftResult = fft.computeFFT3D(converted, sampleRate);
        auto magSpectrum = fft.computeMagnitudeFFT(converted, sampleRate);

        std::cout << "FFT Results:" << std::endl;
        std::cout << "Size of frequencies vector bins: " << fftResult.frequencies.size() << std::endl;
        std::cout << "Last frequency bin value (max spectrum frequency): " << fftResult.frequencies[fftResult.frequencies.size()-1] << std::endl;

        double dt_ns = 1'000'000'000ULL / sampleRate;
        auto start_time = std::chrono::steady_clock::now().time_since_epoch();
        uint64_t start_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(start_time).count();

        MqttClient mqttClient(mqttCfg.broker,
                            mqttCfg.client_id,
                            mqttCfg.topic);

        mqttClient.connect();

        mqttClient.publishWaveform(converted.xs, converted.ys, converted.zs, start_ns, dt_ns);

        mqttClient.publishFFT(fftResult.frequencies, fftResult.magnitudeTotal, fftResult.magnitudeX,
                            fftResult.magnitudeY,
                            fftResult.magnitudeZ);

        mqttClient.disconnect();


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}