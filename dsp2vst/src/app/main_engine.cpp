#include "audio/AudioEngine.h"
#include "effects/FfDelay.h"

int main() {
    AudioEngine engine("./input.wav", "output.wav", 1024);

    // Add ur effects here

    engine.addEffect(std::make_unique<FfDelay>(22000, 1.0f, 44100));
    engine.run();
    return 0;
}