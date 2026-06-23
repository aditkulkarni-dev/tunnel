#include "AudioEngine.h"
#include "../io/AudioFileIO.h"
#include "effects/EffectChain.h"
#include "AudioBuffer_opt.h"
#include <iostream>

AudioEngine::AudioEngine(const std::string& inputPath,
                         const std::string& outputPath,
                         size_t blockSize)
    : inputPath(inputPath), outputPath(outputPath), blockSize(blockSize) {}

void AudioEngine::addEffect(std::unique_ptr<Effect> effect) {
    templateChain.addEffect(std::move(effect));
}

void AudioEngine::run(){

    AudioFileIO fileIO;

    // audio should already be planar here so its easy

    std::unique_ptr<AudioBuffer> planarAudioBuffer = fileIO.readWav(inputPath);

    /*
        Cloning: Each channel must get its own independent effect
        instance. So if we have a standard audio broken down into LR channels,
        we need separate effect chains for L and R respectively.
    */
    std::vector<EffectChain> chains;
    int numChannels = planarAudioBuffer->getNumChannels();
    int numSamples = planarAudioBuffer->getNumSamples();
    chains.reserve(numChannels);
    for (int ch=0; ch < numChannels; ++ch){
        chains.push_back(templateChain.clone());
    }

    for (int ch{0}; ch < numChannels; ++ch){
        float* channel_ch = planarAudioBuffer->getWritePtr(ch);
        
        for (int offset{0}; offset < numSamples; offset+=blockSize){
            // in case we have lesser elements than blockSize left, this takes the minimum of two
            // to ensure that we aren't accessing memory which isn't ours
            int currentBlockSize = std::min(static_cast<int>(blockSize), numSamples-offset);
            
            /*
                Channel 0 : [0, 1, 0, 1, 0]
                if we have a block size of 2, 
                the starting ptr would be initially pointing at idx=0
                then it would jump to i=2, and so on

                we treat each block as its own array and take its address
                as a starting point. 
                So we look at [0, 1, 0, 1, 0] as, 
                process (idx=0, 2 samples),
                process(idx=2, 2 samples)
                process(idx=4, 1 sample)
            */
            float* blockStartPtr = &channel_ch[offset];
            chains[ch].process(blockStartPtr, currentBlockSize);

        }
    }
    fileIO.writeWav(outputPath, *planarAudioBuffer);


    // OLDER CODE !!!

    // // Read
    // AudioFileIO fileIO;
    // AudioBuffer clip = fileIO.readWav(inputPath);
    // const int channels = clip.numChannels;
    // std::cout << "Loaded: " << inputPath << "\n";
    // std::cout << "Channels: " << channels << "  Frames: "
    // << clip.numFrames << "  Sample rate: " << clip.sampleRate << "\n";

    // // Deinterleave into one mono-buffer per channel
    // std::vector<AudioBuffer> channelClips = deinterleave(clip);

    

    // // Split channel into buffers/blocks
    // std::vector<std::vector<AudioBuffer>> blocks;
    // blocks.reserve(channels);
    // for (int ch = 0; ch < channels; ++ch) {
    //     blocks.push_back(splitIntoBlocks(channelClips[ch], blockSize));
    // }

    // /*
    //     Note that processing below occurs 
    //     on arrays with the size of the buffer size.
    // */
    // for (int ch = 0; ch < channels; ++ch) {
    //     for (auto& block : blocks[ch]) {
    //         chains[ch].process(block);
    //     }
    // }

    // std::vector<AudioBuffer> finalClips;
    // finalClips.reserve(channels);
    // for (int ch = 0; ch < channels; ++ch) {
    //     finalClips.push_back(mergeBlocks(blocks[ch]));
    // }

    // AudioBuffer finalClip = interleave(finalClips);
    // fileIO.writeWav(outputPath, finalClip);

    // std::cout << "Wrote: " << outputPath << "\n";

}