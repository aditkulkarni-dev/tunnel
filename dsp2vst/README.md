## DSP2VST
DSP2VST is a DSP-first audio development framework that allows developers to prototype audio effects in standard C++, test them offline, and export them as a JUCE-based VST3 plugin.
  
Instead of writing directly inside a plugin environment, DSP2VST lets you:
1) Write DSP code in plain C++
2) Test effects on audio files offline
3) Export the same effect into a VST3 plugin via JUCE

## Prerequisites
Windows 10 or higher
Visual Studio 2022 or higher
CMake 3.22+
vcpkg
Git
## Installation
1) Install Visual Studio and VSCode
2) Install vcpkg and integrate it with CMake
3) Install dependencies, mainly sndfile for audio file IO
4) Clone repository
5) Configure project with CMake
6) Build

## Quick Start 

### Creating a custom effect
All user defined effects must inherit from the `Effect` base class:
```
class MyEffect : public Effect {
public:
    void process(float* channelData, int numSamples) override {
        for (int i = 0; i < numSamples; ++i) {
            channelData[i] *= 0.5f; // example: simple gain
        }
    }

    std::unique_ptr<Effect> clone() const override {
        return std::make_unique<MyEffect>(*this);
    }
};
```
### Effect Interface
Each effect must implement:
- `process(float* channelData, int numSamples)`
	- Processes audio in-place
	- `channelData` points to the current audio buffer for a single channel
	- `numSamples` is the number of samples in the buffer block
- `clone()`
	- Returns a `std::unique_ptr<Effect>`
	- Must return a deep copy of the effect
	- Required for per-channel duplication (important for stateful effects)
### Key Design Idea
DSP2VST treats effects as modular DSP units that can be:
- Chained in offline processing
- replicated per channel
- automatically ported into a plugin architecture (JUCE for example)
### Why this exists
JUCE plugin development tightly couples DSP logic with UI and plugin lifecycle.
DSP2VST separates concerns:
- DSP development stays independent
- Plugin wrapping is handled later in the pipeline

### Development pipeline for an effect plugin
- Write your custom effect
- Test the effect offline using the offline audio engine
- Run the `translator.py`
- Upon the generation of JUCE compatible source files, set the PluginName and EffectName inside CMake
- Build

### Example usages of tools
To process an audio with a given effect, the developer will have to mainly write their Effect separately inside a header file and the `main_engine.cpp` would look something like:
```
#include "audio/AudioEngine.h"
#include "effects/FfDelay.h"

int main() {
    AudioEngine engine("./input.wav", "output.wav", 1024);

    // Add ur effects here

    engine.addEffect(std::make_unique<FfDelay>(22000, 1.0f, 44100));
    engine.run();
    return 0;
}
```
To translate the user-defined effect into JUCE-compatible code, the developer will have to make some changes in the `translator.py` by replacing the path variables inside the constructor:
```
if __name__ == "__main__":
    SRC_EFFECT_HEADER = "../effects/FfDelay.h"
    JUCE_TEMPLATE_DIR = "../juce"
    generate_juce_project(SRC_EFFECT_HEADER, JUCE_TEMPLATE_DIR)
```

