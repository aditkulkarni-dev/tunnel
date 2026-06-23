#pragma once
#include "../audio/AudioBuffer_opt.h"
#include <memory>

class Effect{
public:
    virtual ~Effect() = default;
    // Every effect must have a process
    // An effect must not own the memory. Only the engine owns memory
    virtual void process(float* data, int numSamples) = 0;

    virtual std::unique_ptr<Effect> clone() const = 0;
    
};