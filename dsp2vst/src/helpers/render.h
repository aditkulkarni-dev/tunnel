#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include "../RTClasses/SparseIR.h"
#include <sndfile.h>

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
}