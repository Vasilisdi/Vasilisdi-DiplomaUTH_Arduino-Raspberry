#pragma once
#include <vector>
#include <cstdint>
#include <complex>

struct ConvertedData {
    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> zs;
};

struct FFTResult {
    std::vector<double> frequencies;  // frequency bins
    std::vector<double> magnitudeX;
    std::vector<double> magnitudeY;
    std::vector<double> magnitudeZ;
    std::vector<double> magnitudeTotal;   // ← new: FFT of sqrt(x²+y²+z²)
};

class Converter {
public:
    ConvertedData convert(const std::vector<uint16_t>& xs,
                          const std::vector<uint16_t>& ys,
                          const std::vector<uint16_t>& zs);
private:
    double toG(uint16_t value);
};

class FFTProcessor {
public:
    // Compute FFT for a single vector (real input), returns magnitude.
    // Data length must be a power of two (or we will zero‑pad to next power of two).
    std::vector<double> computeMagnitude(const std::vector<double>& data);

    // Compute FFT for all three axes and return FFTResult (frequencies + magnitude per axis)
    FFTResult computeFFT3D(const ConvertedData& data, double sampleRate);

    // New: compute magnitude signal sqrt(x²+y²+z²) and its FFT
    // fftLen: optional – if 0, uses next power of two of the data length.
    std::vector<double> computeMagnitudeFFT(const ConvertedData& data, double sampleRate, size_t fftLen = 0);

private:
    // Iterative Cooley–Tukey FFT (in‑place, complex input/output)
    void fft(std::vector<std::complex<double>>& x, bool invert);
};