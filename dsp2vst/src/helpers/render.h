#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include "../RTClasses/SparseIR.h"
#include <sndfile.h>
#include <sstream>
#include <filesystem>

void renderDenseIRFractional(const SparseIR& sparseIR, 
    float sampleRate = 44100.0f, const char* filename = "myIR.wav"){
    if (sparseIR.delays.empty()) {return;}

    int maxIndex{0};
    for (size_t i = 0; i < sparseIR.delays.size(); ++i) {
        float floatIdx = sparseIR.getSampleIndexFloat(i, sampleRate);
        int upperNeighbor = static_cast<int>(std::ceil(floatIdx));
        if (upperNeighbor > maxIndex) {
            maxIndex = upperNeighbor;
        }
    }

    std::vector<float> denseBuffer(maxIndex + 2, 0.0f);
    float maxAmplitude = 0.0f;

    for(size_t i{0}; i < sparseIR.delays.size(); ++i){
        float floatIdx = sparseIR.getSampleIndexFloat(i, sampleRate);
        if (floatIdx < 0.0f){continue;}

        int idxLower = static_cast<int>(std::floor(floatIdx));
        int idxUpper = idxLower + 1;
        float fraction = floatIdx - static_cast<float>(idxLower);
        float totalGain = sparseIR.gains[i];
        denseBuffer[idxLower] += (1.0f - fraction) * totalGain;
        denseBuffer[idxUpper] += fraction * totalGain;
    }

    for (const float& sample : denseBuffer) {
        if (std::abs(sample) > maxAmplitude) {
            maxAmplitude = std::abs(sample);
        }
    }

    if (maxAmplitude > 0.0f) {
        for (float& sample : denseBuffer) {
            sample /= maxAmplitude;
        }
    }

    SF_INFO info{};
    info.channels = 1;
    info.samplerate = static_cast<int>(sampleRate);
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    SNDFILE* file = sf_open(filename, SFM_WRITE, &info);
    
    if (!file)
    {
        std::cerr << "Failed to open WAV file: "
                << sf_strerror(nullptr) << '\n';
        return;
    }

    sf_count_t written = sf_write_float(file,
                                        denseBuffer.data(),
                                        denseBuffer.size());

    std::cout << "Wrote " << written << " samples\n";

    sf_close(file);
    std::cout << std::filesystem::exists(filename) << '\n';
}

void renderDenseIRFractionalCSV(const std::string& csvPath,
                                float sampleRate = 44100.0f,
                                const char* filename = "myIR.wav")
{
    std::vector<float> delays;
    std::vector<float> gains;

    // ---------------------------
    // Read CSV
    // ---------------------------
    std::ifstream file(csvPath);

    if (!file.is_open()) {
        std::cerr << "Failed to open CSV file: "
                  << csvPath << '\n';
        return;
    }

    std::string line;

    // Skip header if present
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);

        std::string delayStr;
        std::string gainStr;

        if (!std::getline(ss, delayStr, ',')) {
            continue;
        }

        if (!std::getline(ss, gainStr, ',')) {
            continue;
        }

        delays.push_back(std::stof(delayStr));
        gains.push_back(std::stof(gainStr));
    }

    if (delays.empty()) {
        return;
    }

    // ---------------------------
    // Find required buffer size
    // ---------------------------
    int maxIndex = 0;

    for (float delay : delays)
    {
        float floatIdx = delay * sampleRate;
        int upperNeighbor = static_cast<int>(std::ceil(floatIdx));

        if (upperNeighbor > maxIndex) {
            maxIndex = upperNeighbor;
        }
    }

    std::vector<float> denseBuffer(maxIndex + 2, 0.0f);

    // ---------------------------
    // Fractional delay rendering
    // ---------------------------
    for (size_t i = 0; i < delays.size(); ++i)
    {
        float floatIdx = delays[i] * sampleRate;

        if (floatIdx < 0.0f) {
            continue;
        }

        int idxLower = static_cast<int>(std::floor(floatIdx));
        int idxUpper = idxLower + 1;

        float fraction = floatIdx - static_cast<float>(idxLower);

        denseBuffer[idxLower] += (1.0f - fraction) * gains[i];
        denseBuffer[idxUpper] += fraction * gains[i];
    }

    // ---------------------------
    // Normalize
    // ---------------------------
    float maxAmplitude = 0.0f;

    for (float sample : denseBuffer)
    {
        if (std::abs(sample) > maxAmplitude) {
            maxAmplitude = std::abs(sample);
        }
    }

    if (maxAmplitude > 0.0f)
    {
        for (float& sample : denseBuffer) {
            sample /= maxAmplitude;
        }
    }

    // ---------------------------
    // Write WAV
    // ---------------------------
    SF_INFO info{};
    info.channels = 1;
    info.samplerate = static_cast<int>(sampleRate);
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* wavFile = sf_open(filename, SFM_WRITE, &info);

    if (!wavFile)
    {
        std::cerr << "Failed to open WAV file: "
                  << sf_strerror(nullptr) << '\n';
        return;
    }

    sf_count_t written = sf_write_float(
        wavFile,
        denseBuffer.data(),
        denseBuffer.size()
    );

    std::cout << "Wrote " << written
              << " samples\n";

    sf_close(wavFile);
}