#include <windows.h>
#include <iostream>
#include <vector>
#include <cstdint>
#include <chrono>

void printDCB(const DCB& dcb) {
    std::cout << "DCB Parameters:\n";
    std::cout << "  BaudRate: " << dcb.BaudRate << "\n";
    std::cout << "  ByteSize: " << (int)dcb.ByteSize << "\n";
    std::cout << "  Parity:   " << (int)dcb.Parity << "\n";
    std::cout << "  StopBits: " << (int)dcb.StopBits << "\n";
}

int main() {

    HANDLE serial = CreateFile("\\\\.\\COM5", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (serial == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening this specific COM port\n";
        return 1;
    }

    DCB dcb ={0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(serial, &dcb);
    printDCB(dcb); 
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    printDCB(dcb); 

    SetCommState(serial, &dcb);


    COMMTIMEOUTS t = {0};
    t.ReadIntervalTimeout = 50;
    t.ReadTotalTimeoutConstant = 50;
    SetCommTimeouts(serial, &t);


    std::vector<uint16_t> xs, ys, zs;

    auto start = std::chrono::steady_clock::now(); 

    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(60)) 
    {
        uint8_t buffer[60];
        DWORD bytesRead = 0;

        ReadFile(serial, buffer, sizeof(buffer), &bytesRead, NULL);

        int samples = bytesRead / 6;
        if (bytesRead == 60) 
        {
            for (int i = 0; i < samples; i++)
            {
                uint16_t x = (buffer[i*6 + 0] << 8) | buffer[i*6 + 1];
                uint16_t y = (buffer[i*6 + 2] << 8) | buffer[i*6 + 3];
                uint16_t z = (buffer[i*6 + 4] << 8) | buffer[i*6 + 5];

                xs.push_back(x);
                ys.push_back(y);
                zs.push_back(z);
            }
        } else if (bytesRead != 60)
        {
            std::cout << "Incomplete packet\n";
            std::cout << bytesRead;
        }
    }

    CloseHandle(serial);

    for (size_t i = 0; i < xs.size() ; i++)
    {
        std::cout << xs[i] << " " << ys[i] << " " << zs[i] << "\n";
    }

    std::cout << "Total samples length: " << xs.size() << "\n";

    return 0;

}