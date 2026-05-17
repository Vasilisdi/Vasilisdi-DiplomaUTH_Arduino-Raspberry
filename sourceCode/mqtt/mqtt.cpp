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

std::string getDateTag() {
    auto t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    char buf[32];
    std::strftime(buf, sizeof(buf), "%b_%d", &tm);
    
    // μετατροπή σε lowercase
    std::string s(buf);
    for (auto &c : s) c = std::tolower(c);

    return s;
}


std::string MqttClient::createWaveJson(uint64_t ts, double x, double y, double z)
{
    return "{\"t\":" + std::to_string(ts) +
           ",\"v_x\":" + std::to_string(x) + 
           ",\"v_y\":" + std::to_string(y) + 
           ",\"v_z\":" + std::to_string(z) + "}";
}

std::string MqttClient::createFFTJson(double frequencies, double magTotal, 
                           double magX,  double magY, double magZ)
{

    return "{\"frequency\":" + std::to_string(frequencies) +
                ",\"FFT_total\":" + std::to_string(magTotal) +
                ",\"FFT_x\":" + std::to_string(magX) +
                ",\"FFT_y\":" + std::to_string(magY) + 
                ",\"FFT_z\":" + std::to_string(magZ) + "}";
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
    
    std::vector<double> vec;
    std::vector<uint64_t> ts;

    for (size_t i = 0; i < xs.size(); ++i) {
        double o = i;
        vec.push_back(o);
        uint64_t ts_i = start_ns + static_cast<uint64_t>(i * dt_ns);
        ts.push_back(ts_i);
    }

    for (size_t i = 0; i < 2000; i++)
    {
        std::string json = createWaveJson(ts[i],xs[i],ys[i], zs[i]);
        std::string dateTag = getDateTag();
        std::string topic = topic_ + "/waveform/" +  getDateTag();

        auto msg = mqtt::make_message(topic, json);
        msg->set_qos(1);

        std::cout << "chunkTopic = " << topic << std::endl;
        client_.publish(msg)->wait();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

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

    for (size_t i = 0; i < 35000; i++)
    {
        std::string json = createFFTJson(frequencies[i] , magTotal[i] , magX[i] , magY[i] , magZ[i]);
        std::string dateTag = getDateTag();
        std::string chunkTopic = topic_ + "/frequencies/" +  getDateTag();
        auto msg = mqtt::make_message(chunkTopic, json);
        std::cout << "chunkTopic = " << chunkTopic << std::endl; 
        msg->set_qos(1);
        client_.publish(msg)->wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "Published waveform: " << frequencies.size() << " samples per axis\n";  


}

    