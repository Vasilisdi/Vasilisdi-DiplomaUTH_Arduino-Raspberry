#define _USE_MATH_DEFINES
#include "analysis.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

// -----------------------------------------------------------------------------
// Converter implementation (unchanged)
// -----------------------------------------------------------------------------
double Converter::toG(uint16_t value) {
    return (static_cast<double>(value) / 4095.0) * 6.0 - 3.0;
}

ConvertedData Converter::convert(const std::vector<uint16_t>& xs,
                                 const std::vector<uint16_t>& ys,
                                 const std::vector<uint16_t>& zs) {
    ConvertedData result;
    result.xs.reserve(xs.size());
    result.ys.reserve(ys.size());
    result.zs.reserve(zs.size());

    for (size_t i = 0; i < xs.size(); ++i) {
        result.xs.push_back(toG(xs[i]));
        result.ys.push_back(toG(ys[i]));
        result.zs.push_back(toG(zs[i]));
    }
    return result;
}

// -----------------------------------------------------------------------------
// FFTProcessor implementation
// -----------------------------------------------------------------------------
void FFTProcessor::fft(std::vector<std::complex<double>>& x, bool invert) {
    size_t n = x.size();
    if (n == 1) return;

    // Bit-reversal permutation
    for (size_t i = 1, j = 0; i < n; ++i) {
        size_t bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
            std::swap(x[i], x[j]);
    }

    // FFT stages
    for (size_t len = 2; len <= n; len <<= 1) {
        double angle = 2 * M_PI / len * (invert ? -1 : 1);
        std::complex<double> wlen(std::cos(angle), std::sin(angle));
        for (size_t i = 0; i < n; i += len) {
            std::complex<double> w(1);
            for (size_t j = 0; j < len / 2; ++j) {
                std::complex<double> u = x[i + j];
                std::complex<double> v = x[i + j + len / 2] * w;
                x[i + j] = u + v;
                x[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (size_t i = 0; i < n; ++i)
            x[i] /= n;
    }
}

std::vector<double> FFTProcessor::computeMagnitude(const std::vector<double>& data) {
    // Find next power of two
    size_t n = 1;
    while (n < data.size()) n <<= 1;

    // Copy data into complex vector (real part only)
    std::vector<std::complex<double>> complexData(n, std::complex<double>(0, 0));
    for (size_t i = 0; i < data.size(); ++i)
        complexData[i] = std::complex<double>(data[i], 0);

    // Perform FFT
    fft(complexData, false);

    // Compute magnitude for positive frequencies (0 to n/2)
    std::vector<double> magnitude(n / 2 + 1);
    for (size_t i = 0; i <= n / 2; ++i) {
        magnitude[i] = std::abs(complexData[i]);
    }
    // Scale interior bins (1 to n/2 - 1) by 2 to get one‑sided spectrum
    for (size_t i = 1; i < n / 2; ++i) {
        magnitude[i] *= 2.0;
    }
    return magnitude;
}

std::vector<double> FFTProcessor::computeMagnitudeFFT(const ConvertedData& data, double sampleRate, size_t fftLen) {
    // 1. Compute magnitude per sample sqrt(x²+y²+z²)
    size_t n = data.xs.size();
    std::vector<double> magnitude(n);
    for (size_t i = 0; i < n; ++i) {
        double x = data.xs[i];
        double y = data.ys[i];
        double z = data.zs[i];
        magnitude[i] = std::sqrt(x*x + y*y + z*z);
    }

    // 2. Determine FFT length (next power of two if fftLen == 0)
    size_t len = fftLen;
    if (len == 0) {
        len = 1;
        while (len < n) len <<= 1;
    } else {
        // ensure len is a power of two (or we could zero‑pad anyway)
        size_t p2 = 1;
        while (p2 < len) p2 <<= 1;
        len = p2;
    }

    // 3. Zero‑pad and copy to complex array
    std::vector<std::complex<double>> complexData(len, std::complex<double>(0, 0));
    for (size_t i = 0; i < n; ++i)
        complexData[i] = std::complex<double>(magnitude[i], 0);

    // 4. FFT
    fft(complexData, false);

    // 5. One‑sided magnitude spectrum (positive frequencies)
    size_t half = len / 2;
    std::vector<double> magSpectrum(half + 1);
    for (size_t i = 0; i <= half; ++i) {
        magSpectrum[i] = std::abs(complexData[i]);
    }
    // Scale interior bins
    for (size_t i = 1; i < half; ++i) {
        magSpectrum[i] *= 2.0;
    }
    return magSpectrum;
}

FFTResult FFTProcessor::computeFFT3D(const ConvertedData& data, double sampleRate) {
    FFTResult result;

    // Compute per‑axis spectra
    result.magnitudeX = computeMagnitude(data.xs);
    result.magnitudeY = computeMagnitude(data.ys);
    result.magnitudeZ = computeMagnitude(data.zs);

    // Compute total magnitude spectrum (sqrt(x²+y²+z²))
    result.magnitudeTotal = computeMagnitudeFFT(data, sampleRate);

    // Frequency bins (same for all)
    size_t n = (result.magnitudeX.size() - 1) * 2;   // original FFT length
    result.frequencies.resize(result.magnitudeX.size());
    for (size_t i = 0; i < result.magnitudeX.size(); ++i) {
        result.frequencies[i] = static_cast<double>(i) * sampleRate / n;
    }
    return result;
}