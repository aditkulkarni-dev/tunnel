#include "FfDelay.h"

void FfDelay::setBufferSize(int bufferSize){
    buffer.setSize(bufferSize);
}

void FfDelay::process(float* data, int numSamples){
    
    for (int i{0}; i < numSamples; ++i){
        float input = data[i];
        
        //Read the old sample from 'delay' samples ago
        float delayed = buffer.read(delay);
 
        //Mix the dry input with the wet delayed signal
        float output = input + mix * delayed;
        
        //Store the current input sample into the buffer
        buffer.write(input);
        
        //Update the audio block array
        data[i] = output;
        
        //Step the buffer pointer forward for the next iteration!
        buffer.advance(); 
    }
}

std::unique_ptr<Effect> FfDelay::clone() const{
    return std::make_unique<FfDelay>(*this); 
}

std::vector<AudioParameter<FfDelay>> const FfDelay::getAudioParameters()
{
    return params;
}

void FfDelay::setDelay(float newDelay)
{
    delay = static_cast<int>(newDelay);
}

void FfDelay::setMix(float newMix)
{
    mix = newMix;
}
