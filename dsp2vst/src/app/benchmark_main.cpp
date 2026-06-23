#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <memory>
#include <string>
#include <cstdio> // For std::remove
#include <sndfile.h>
#include "audio/AudioEngine.h"
#include "effects/ffDelay.h"

// Forward declaration so main() knows it exists!
std::string f_left(std::string str, size_t width);

// Helper function to programmatically generate dummy audio files for testing
void generateDummyWav(const std::string& filename, int numSamples, int sampleRate = 44100, int channels = 2) {
    SF_INFO info{};
    info.samplerate = sampleRate;
    info.channels = channels;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &info);
    if (!file) return;

    // Write a quick silent block
    std::vector<float> buffer(numSamples * channels, 0.0f);
    sf_writef_float(file, buffer.data(), numSamples);
    sf_close(file);
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "      AUDIO ENGINE NATIVE PERFORMANCE BENCHMARK   \n";
    std::cout << "==================================================\n";

    // Test durations in seconds (e.g., 5s, 10s, 30s, 60s, 120s, 180s)
    std::vector<int> testDurations = {5, 10, 30, 60, 120, 180};
    int sampleRate = 44100;

    // Open a CSV file to save the data points for plotting later
    std::ofstream csvFile("benchmark_results.csv");
    csvFile << "Samples,RuntimeMS\n";

    std::cout << f_left("Samples (N)", 15) << " | " << f_left("Duration (s)", 15) << " | " << "Runtime (ms)\n";
    std::cout << "--------------------------------------------------\n";

    for (int duration : testDurations) {
        int numSamples = duration * sampleRate;
        std::string inputPath = "temp_bench_input.wav";
        std::string outputPath = "temp_bench_output.wav";

        // 1. Generate the file
        generateDummyWav(inputPath, numSamples, sampleRate);

        // 2. Instantiate and configure the engine
        AudioEngine engine(inputPath, outputPath);
        engine.addEffect(std::make_unique<FfDelay>(22000, 1.0f, sampleRate));

        // 3. Profile the engine run
        auto start = std::chrono::high_resolution_clock::now();
        engine.run();
        auto end = std::chrono::high_resolution_clock::now();

        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        // Output to terminal
        std::cout << f_left(std::to_string(numSamples), 15) << " | " 
                  << f_left(std::to_string(duration) + "s", 15) << " | " 
                  << durationMs << " ms\n";

        // Save data point to CSV
        csvFile << numSamples << "," << durationMs << "\n";

        // Clean up temp files immediately
        std::remove(inputPath.c_str());
        std::remove(outputPath.c_str());
    }

    csvFile.close();
    std::cout << "--------------------------------------------------\n";
    std::cout << "Benchmark data successfully saved to 'benchmark_results.csv'\n";

    return 0;
}

// Quick string formatter helper for clean terminal spacing
std::string f_left(std::string str, size_t width) {
    if (str.size() >= width) return str;
    return str + std::string(width - str.size(), ' ');
}