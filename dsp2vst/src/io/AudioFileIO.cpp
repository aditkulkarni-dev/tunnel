#include "AudioFileIO.h"
#include <sndfile.h>
#include <stdexcept>
#include "../audio/AudioBuffer_opt.h"
#include <memory>
#include <iostream>

std::unique_ptr<AudioBuffer> AudioFileIO::readWav(const std::string& filePath){
    SF_INFO info{};
    SNDFILE* file = sf_open(filePath.c_str(), SFM_READ, &info);

    if (!file) throw std::runtime_error("Failed to open file");

    // initialise buffer
    
    auto buffer = std::make_unique<AudioBuffer>(info.frames, info.channels, info.samplerate);
    std::vector<float> temporaryData(info.frames * info.channels, 0.0f);

    sf_readf_float(file, temporaryData.data(), info.frames);
    sf_close(file);

    for(int ch{0}; ch < info.channels; ++ch){
        float* channel_ch = buffer->getWritePtr(ch);
        for(int i{0}; i < (info.frames); ++i){
            channel_ch[i] = temporaryData[ch + i*info.channels];
        }
    }
    return buffer;
}

void AudioFileIO::writeWav(const std::string& filePath, const AudioBuffer& buffer) {
    SF_INFO info{};
    info.samplerate = buffer.getSampleRate(); 
    info.channels = buffer.getNumChannels();
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    // We do NOT need to set info.frames for SFM_WRITE. libsndfile ignores it 
    // on input and overwrites it with 0 anyway.
    
    // Store the frame count locally so it doesn't get overwritten!
    const int numFrames = buffer.getNumSamples();

    SNDFILE* file = sf_open(filePath.c_str(), SFM_WRITE, &info);
    

    if (!file) throw std::runtime_error("Failed to open file");

    // Allocate using our local variable
    std::vector<float> temporaryData(numFrames * info.channels, 0.0f);

    // The Interleaving: Zip the planar channels back into 1D
    for (int ch = 0; ch < info.channels; ++ch) {
        const float* channel_ch = buffer.getReadPtr(ch);
        
        // Loop using our local variable
        for (int i = 0; i < numFrames; ++i) {
            temporaryData[ch + i * info.channels] = channel_ch[i];
        }
    }

    // Write using our local variable
    sf_writef_float(file, temporaryData.data(), numFrames);
    sf_close(file);
}