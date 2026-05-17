#include "mqtt.hpp"
#include <iostream>
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
                                   const std::vector<double>& x,
                                   const std::vector<double>& y,
                                   const std::vector<double>& z,
                                   uint64_t start_ns,
                                   double dt_ns) 
{
    std::string json = "[";
    for (size_t i = start; i < end; ++i) 
    {
        if (i > start) json += ",";

        uint64_t ts = start_ns + (i * dt_ns);

        json += "{\"t\":" + std::to_string(ts) +
                ",\"v_x\":" + std::to_string(x[i]) +
                ",\"v_y\":" + std::to_string(y[i]) + 
                ",\"v_z\":" + std::to_string(z[i]) + "}";
    }
    json += "]";
    return json;
}



std::string MqttClient::createTimeJson(size_t start, size_t end,
                           uint64_t start_ns,
                           double dt_ns)
{
    std::string json = "[";
    for (size_t i = start; i < end; ++i) 
    {
        if (i > start) json += ",";

        uint64_t ts = start_ns + (i * dt_ns);

        json += "{\"t\":" + std::to_string(ts) +
                ",\"v\":" + std::to_string(ts) + "}";
    }
    json += "]";
    return json;
}




std::string MqttClient::createWaveJson(size_t start, size_t end,
                           const std::vector<double>& s)
{
    std::string json = "{\"v\":[";
    for (size_t i = start; i < end; ++i)
    {
        if (i > start) json += ",";
        json += std::to_string(s[i]);
    }
    json += "]}";
    return json;
}

std::string MqttClient::createFFTJson(size_t start, size_t end,
                           const std::vector<double>& frequencies, 
                           const std::vector<double>& magTotal, 
                           const std::vector<double>& magX, 
                           const std::vector<double>& magY, 
                           const std::vector<double>& magZ)
{
    std::string json = "[";
    for (size_t i = start; i < end; ++i) 
    {
        if (i > start) json += ",";

        json += "{\"frequency\":" + std::to_string(frequencies[i]) +
                ",\"FFT_total\":" + std::to_string(magTotal[i]) +
                ",\"FFT_x\":" + std::to_string(magX[i]) +
                ",\"FFT_y\":" + std::to_string(magY[i]) + 
                ",\"FFT_z\":" + std::to_string(magZ[i]) + "}";
    }
    json += "]";
    return json;
}

///////////////////////////////////////////////////////////////////////////////
// WAVEFORM → SINGLE SNAPSHOT (ARRAY FORMAT)
///////////////////////////////////////////////////////////////////////////////
void MqttClient::publishWaveform(
    const std::vector<double>& xs,
    const std::vector<double>& ys,
    const std::vector<double>& zs,
    uint64_t start_ns,
    double dt_ns)
{
    if (xs.empty()) return;

    const size_t CHUNK_SIZE = 3000;
    uint16_t last_chunk = 0;
    size_t numChunks = (xs.size() + CHUNK_SIZE - 1) / CHUNK_SIZE; //ceiling devision to make sure xs.size is smaller
    
    for (size_t chunk = 0; chunk < numChunks; chunk++) 
    {
        size_t startIdx = chunk * CHUNK_SIZE;
        size_t endIdx = std::min(startIdx + CHUNK_SIZE, xs.size());

        uint64_t chunk_start_ns = start_ns + static_cast<uint64_t>(startIdx * dt_ns);


        // t,x,y,z
        std::string json = createJson(startIdx, endIdx, xs , ys , zs, chunk_start_ns, dt_ns);
        std::string chunkTopic = topic_ + "/waveform/" + std::to_string(chunk);
        auto msg = mqtt::make_message(chunkTopic, json);
        std::cout << "chunkTopicx = " << chunkTopic << std::endl; 
        msg->set_qos(1);
        client_.publish(msg)->wait();

        last_chunk = chunk;

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    std::cout << "Total number of chunks " << last_chunk + 1 << " / " << numChunks  << std::endl;

    std::cout << "Published waveform: " << xs.size() << " samples per axis\n";
}





///////////////////////////////////////////////////////////////////////////////
// FFT → SINGLE SNAPSHOT (STRING ARRAYS)
///////////////////////////////////////////////////////////////////////////////
void MqttClient::publishFFT(const std::vector<double>& frequencies,
                            const std::vector<double>& magTotal,
                            const std::vector<double>& magX,
                            const std::vector<double>& magY,
                            const std::vector<double>& magZ) {

    if (frequencies.empty()) return;
    
    const size_t CHUNK_SIZE = 3000;
    uint16_t last_chunk = 0;
    size_t numChunks = (frequencies.size() + CHUNK_SIZE - 1) / CHUNK_SIZE; //ceiling devision
    std::string last_json;

    for (size_t chunk = 0; chunk < numChunks; chunk++) 
    {
        size_t startIdx = chunk * CHUNK_SIZE;
        size_t endIdx = std::min(startIdx + CHUNK_SIZE, frequencies.size());

        // frequencies, FFT_x. FFT_y, FFT_z
        std::string json = createFFTJson(startIdx, endIdx, frequencies , magTotal , magX , magY , magZ);
        last_json = json;  // store last chunk
        std::string chunkTopic = topic_ + "/frequencies/" + std::to_string(chunk);
        auto msg = mqtt::make_message(chunkTopic, json);
        std::cout << "chunkTopicx = " << chunkTopic << std::endl; 
        msg->set_qos(1);
        client_.publish(msg)->wait();

        last_chunk = chunk;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    std::cout << "Total number of chunks " << last_chunk + 1 << " / " << numChunks  << std::endl;

    std::cout << "Published waveform: " << frequencies.size() << " samples per axis\n";  

    //std::cout << "Actual JSON:\n" << last_json << "\n";

}

    