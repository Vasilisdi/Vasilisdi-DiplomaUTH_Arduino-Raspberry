#pragma once
#include <string>
#include <vector>
#include <mqtt/async_client.h>

class MqttClient {
public:
    MqttClient(const std::string& broker,
               const std::string& client_id,
               const std::string& topic);

    void connect();
    void disconnect();

    void publishWaveform(const std::vector<double>& xs,
                    const std::vector<double>& ys,
                    const std::vector<double>& zs,
                    uint64_t start_ns,
                    double dt_ns);

    void publishFFT(const std::vector<double>& frequencies,
                    const std::vector<double>& magTotal,
                    const std::vector<double>& magX,
                    const std::vector<double>& magY,
                    const std::vector<double>& magZ);

private:
    mqtt::async_client client_;
    std::string topic_;

    std::string createJson(size_t start, size_t end,
                            const std::vector<double>& x , 
                            const std::vector<double>& y ,     
                            const std::vector<double>& z , 
                            uint64_t start_ns,
                            double dt_ns);

    std::string createWaveJson(size_t start, size_t end,
                           const std::vector<double>& s);
    
    std::string createTimeJson(size_t start, size_t end,
                           uint64_t start_ns,
                           double dt_ns);

    std::string MqttClient::createFFTJson(size_t start, size_t end,
                           const std::vector<double>& frequencies, 
                           const std::vector<double>& magTotal, 
                           const std::vector<double>& magX, 
                           const std::vector<double>& magY, 
                           const std::vector<double>& magz);
};