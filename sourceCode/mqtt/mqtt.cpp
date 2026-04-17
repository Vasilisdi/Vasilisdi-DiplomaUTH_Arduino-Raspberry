#include "mqtt.hpp"
#include <iostream>
#include <thread>
#include <chrono>

MqttClient::MqttClient(const std::string& broker,
                       const std::string& client_id,
                       const std::string& topic)
    : client_(broker, client_id), topic_(topic) {}

void MqttClient::connect() {
    mqtt::connect_options opts;
    opts.set_clean_session(true);
    client_.connect(opts)->wait();
}

void MqttClient::disconnect() {
    client_.disconnect()->wait();
}

std::string MqttClient::createJson(size_t start, size_t end,
                                   const std::vector<double>& xs,
                                   const std::vector<double>& ys,
                                   const std::vector<double>& zs) {
    std::string json = "[";
    for (size_t i = start; i < end; ++i) {
        if (i > start) json += ",";
        json += "{\"x\":" + std::to_string(xs[i]) +
                ",\"y\":" + std::to_string(ys[i]) +
                ",\"z\":" + std::to_string(zs[i]) + "}";
    }
    json += "]";
    return json;
}

void MqttClient::publishChunks(const std::vector<double>& xs,
                               const std::vector<double>& ys,
                               const std::vector<double>& zs) {
    const size_t CHUNK_SIZE = 10000;
    uint16_t last_chunk = 0;
    size_t numChunks = (xs.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (size_t chunk = 0; chunk < numChunks; ++chunk) {
        size_t startIdx = chunk * CHUNK_SIZE;
        size_t endIdx = std::min(startIdx + CHUNK_SIZE, xs.size());

        std::string json = createJson(startIdx, endIdx, xs, ys, zs);
        std::string chunkTopic = topic_ + "/" + std::to_string(chunk);
        auto msg = mqtt::make_message(chunkTopic, json);
        msg->set_qos(1);
        client_.publish(msg)->wait();
        last_chunk = chunk;

        // Add a short delay between chunks to smooth the flow
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Total number of chunks " << last_chunk + 1 << std::endl;
}

void MqttClient::publishFFT(const std::vector<double>& frequencies,
                            const std::vector<double>& magTotal,
                            const std::vector<double>& magX,
                            const std::vector<double>& magY,
                            const std::vector<double>& magZ) {
    const size_t CHUNK_SIZE = 10000; // bins per chunk (adjust as needed)
    size_t numChunks = (frequencies.size() + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (size_t chunk = 0; chunk < numChunks; ++chunk) {
        size_t startIdx = chunk * CHUNK_SIZE;
        size_t endIdx = std::min(startIdx + CHUNK_SIZE, frequencies.size());

        // Build JSON for this chunk
        std::string json = "{";
        // freq
        json += "\"freq\":[";
        for (size_t i = startIdx; i < endIdx; ++i) {
            if (i > startIdx) json += ",";
            json += std::to_string(frequencies[i]);
        }
        json += "],";
        // mag_total
        json += "\"mag_total\":[";
        for (size_t i = startIdx; i < endIdx; ++i) {
            if (i > startIdx) json += ",";
            json += std::to_string(magTotal[i]);
        }
        json += "],";
        // mag_x
        json += "\"mag_x\":[";
        for (size_t i = startIdx; i < endIdx; ++i) {
            if (i > startIdx) json += ",";
            json += std::to_string(magX[i]);
        }
        json += "],";
        // mag_y
        json += "\"mag_y\":[";
        for (size_t i = startIdx; i < endIdx; ++i) {
            if (i > startIdx) json += ",";
            json += std::to_string(magY[i]);
        }
        json += "],";
        // mag_z
        json += "\"mag_z\":[";
        for (size_t i = startIdx; i < endIdx; ++i) {
            if (i > startIdx) json += ",";
            json += std::to_string(magZ[i]);
        }
        json += "]}";

        // Publish to chunk topic
        std::string chunkTopic = topic_ + "/fft/" + std::to_string(chunk);
        auto msg = mqtt::make_message(chunkTopic, json);
        msg->set_qos(1);
        client_.publish(msg)->wait();

        // Add a small delay (e.g., 5 ms) between chunks to smooth the flow
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    std::cout << "Published FFT data in " << numChunks << " chunks (" << frequencies.size() << " bins total)" << std::endl;
}