#pragma once
#include <memory>
#include <vector>
#include "../audio/AudioBuffer_opt.h"
#include "Effect.h"

class EffectChain{
public:
    void addEffect(std::unique_ptr<Effect> effectToAdd);
    void process(float* data, int numSamples);
    EffectChain clone() const;

private:
    std::vector<std::unique_ptr<Effect>> effects;

};