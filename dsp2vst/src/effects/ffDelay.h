#include "Effect.h"
#include "../audio/AudioBuffer_opt.h"
#include "../audio/AudioParameter.h"
#include "../dsp/CircularBuffer.h"
#include <memory>
#include <atomic>

class FfDelay : public Effect{
public:
    FfDelay(int delay=22050, float mix = 0.3f, int bufferSize=44100) : delay(static_cast<float>(delay)), mix(mix){
        buffer.setSize(bufferSize);
    }
    void process(float* data, int numSamples) override;
    void setBufferSize(int bufferSize);
    std::unique_ptr<Effect> clone() const override;
    std::vector<AudioParameter<FfDelay>> static const getAudioParameters();
    void setDelay(float newDelay);
    void setMix(float newMix);

private:
    float delay{0};
    float mix{0.3f};
    CircularBuffer buffer;
    inline static const std::vector<AudioParameter<FfDelay>>
     params = {{"delay", "delay", 0, 22050, 1, 11025, &FfDelay::setDelay}, 
                {"mix", "mix", 0.0f, 1.0f, 0.01f, 0.5f, &FfDelay::setMix} };

};
