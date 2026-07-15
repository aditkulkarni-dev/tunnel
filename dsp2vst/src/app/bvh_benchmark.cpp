// benchmark_raytracer.cpp
// THIS BENCHMARKING FILE IS MADE BY CLAUDE, I DO NOT TAKE CREDIT FOR THIS
//
// Standalone benchmark harness for the acoustic ray-tracing / RIR pipeline.
// Measures, in isolation:
//   1. Obj load + BVH build time            (one-shot, unavoidable cost)
//   2. Ray generation time                  (generateRandomVectors)
//   3. simulate() time across a small grid of {rayCount, numBounces}
//
// Design goals:
//   - Keep total wall-clock time short by default (tune the constants below).
//   - Warm up before timing to avoid cold-cache / page-fault noise.
//   - Report min / median / mean / stddev, not just a single number, since
//     thread scheduling noise is exactly what you're chasing with VTune.
//   - No dependency on anything not already in your codebase except <chrono>
//     and friends from the standard library.
//
// Build: add this as its own target (or temporarily swap it for main.cpp).
// It does not write any audio/CSV/OBJ output by default -- it's meant to be
// run repeatedly and cheaply.

#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/VectorGen.h"
#include "../RTClasses/ObjLoader.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <cmath>

// ---------------------------------------------------------------------------
// Tunables -- edit these to trade off benchmark fidelity vs. run time.
// ---------------------------------------------------------------------------
namespace BenchConfig {
    constexpr const char* kObjPath = "sponza_scaled.obj";

    // Ray-generation-only benchmark (cheap, so we can afford more trials).
    const std::vector<int> kRayGenCounts = {1000, 10000, 50000, 100000, 250000, 500000};
    constexpr int kRayGenTrials = 5;

    // Full simulate() benchmark grid. Keep this small -- it's O(rays * bounces)
    // and is the expensive part. 3x3 grid * 2 trials = 18 runs.
    const std::vector<int> kSimRayCounts   = {2000, 10000, 30000};
    const std::vector<int> kSimBounceCounts = {5, 10, 15};
    constexpr int kSimTrials = 2;

    // Listener position used for every run (matches your main.cpp).
    const Vector3D kListenerPos{-2, 0, 2};

    // Fixed source position for every run.
    const Vector3D kSourcePos{0.01f, 0.01f, 0.01f};

    constexpr bool kDoWarmup = true;
    constexpr const char* kCsvOut = "benchmark_results.csv";
}

// ---------------------------------------------------------------------------
// Timing helpers
// ---------------------------------------------------------------------------
struct Stats {
    double minMs = 0, maxMs = 0, meanMs = 0, medianMs = 0, stddevMs = 0;
};

static Stats computeStats(std::vector<double> samplesMs) {
    Stats s;
    if (samplesMs.empty()) return s;
    std::sort(samplesMs.begin(), samplesMs.end());
    s.minMs = samplesMs.front();
    s.maxMs = samplesMs.back();
    s.meanMs = std::accumulate(samplesMs.begin(), samplesMs.end(), 0.0) / samplesMs.size();
    size_t n = samplesMs.size();
    s.medianMs = (n % 2 == 0) ? (samplesMs[n/2 - 1] + samplesMs[n/2]) / 2.0
                               : samplesMs[n/2];
    double sqSum = 0.0;
    for (double v : samplesMs) sqSum += (v - s.meanMs) * (v - s.meanMs);
    s.stddevMs = std::sqrt(sqSum / samplesMs.size());
    return s;
}

class Timer {
public:
    void start() { t0_ = std::chrono::high_resolution_clock::now(); }
    double stopMs() {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0_).count();
    }
private:
    std::chrono::high_resolution_clock::time_point t0_;
};

static void printStatsRow(const std::string& label, const Stats& s, double throughputUnits = -1,
                           const std::string& throughputLabel = "") {
    std::cout << std::left << std::setw(28) << label
              << "min=" << std::setw(9) << std::fixed << std::setprecision(2) << s.minMs
              << "median=" << std::setw(9) << s.medianMs
              << "mean=" << std::setw(9) << s.meanMs
              << "stddev=" << std::setw(9) << s.stddevMs;
    if (throughputUnits >= 0) {
        std::cout << throughputLabel << "=" << std::setprecision(1) << throughputUnits;
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------------------
// CSV output (self-contained, doesn't depend on your existing CSVWriter API)
// ---------------------------------------------------------------------------
struct CsvRow {
    std::string phase;
    int rayCount;
    int numBounces;
    double minMs, medianMs, meanMs, stddevMs;
};

static void writeCsv(const std::string& path, const std::vector<CsvRow>& rows) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cerr << "Warning: could not open " << path << " for benchmark CSV output\n";
        return;
    }
    out << "phase,rayCount,numBounces,minMs,medianMs,meanMs,stddevMs\n";
    for (const auto& r : rows) {
        out << r.phase << "," << r.rayCount << "," << r.numBounces << ","
            << r.minMs << "," << r.medianMs << "," << r.meanMs << "," << r.stddevMs << "\n";
    }
    std::cout << "\nBenchmark results written to " << path << "\n";
}

// ---------------------------------------------------------------------------
// Benchmark phases
// ---------------------------------------------------------------------------
static void benchmarkRayGeneration(std::vector<CsvRow>& csvRows) {
    std::cout << "\n== Ray generation ==\n";
    for (int count : BenchConfig::kRayGenCounts) {
        std::vector<double> samples;
        samples.reserve(BenchConfig::kRayGenTrials);

        if (BenchConfig::kDoWarmup) {
            std::vector<Ray> warm;
            generateRandomVectors(warm, BenchConfig::kListenerPos, count);
        }

        for (int t = 0; t < BenchConfig::kRayGenTrials; ++t) {
            std::vector<Ray> rays;
            Timer timer;
            timer.start();
            generateRandomVectors(rays, BenchConfig::kListenerPos, count);
            samples.push_back(timer.stopMs());
        }

        Stats s = computeStats(samples);
        double raysPerSec = (count / (s.medianMs / 1000.0));
        printStatsRow("gen " + std::to_string(count) + " rays", s, raysPerSec, " rays/s");
        csvRows.push_back({"ray_gen", count, 0, s.minMs, s.medianMs, s.meanMs, s.stddevMs});
    }
}

static void benchmarkSimulate(Scene& scene, std::vector<CsvRow>& csvRows) {
    std::cout << "\n== simulate() ==\n";
    for (int bounces : BenchConfig::kSimBounceCounts) {
        for (int rayCount : BenchConfig::kSimRayCounts) {
            IRSimulationConfig config;
            config.sourcePosition = BenchConfig::kSourcePos;
            config.numBounces = bounces;
            RoomIRSimulator simulator(scene, config);

            std::vector<Ray> rays;
            generateRandomVectors(rays, BenchConfig::kListenerPos, rayCount);

            if (BenchConfig::kDoWarmup) {
                std::vector<std::pair<int, Vector3D>> warmHits;
                simulator.simulate(rays, warmHits);
            }

            std::vector<double> samples;
            samples.reserve(BenchConfig::kSimTrials);
            for (int t = 0; t < BenchConfig::kSimTrials; ++t) {
                std::vector<std::pair<int, Vector3D>> hitPoints;
                Timer timer;
                timer.start();
                SparseIR ir = simulator.simulate(rays, hitPoints);
                double ms = timer.stopMs();
                samples.push_back(ms);
                (void)ir; // discard, we only care about timing here
            }

            Stats s = computeStats(samples);
            double raysPerSec = (rayCount / (s.medianMs / 1000.0));
            std::string label = "rays=" + std::to_string(rayCount) + " bounces=" + std::to_string(bounces);
            printStatsRow(label, s, raysPerSec, " rays/s");
            csvRows.push_back({"simulate", rayCount, bounces, s.minMs, s.medianMs, s.meanMs, s.stddevMs});
        }
    }
}

// ---------------------------------------------------------------------------
int main() {
    std::vector<CsvRow> csvRows;

    std::cout << "== Scene load + BVH build ==\n";
    Scene scene;
    {
        Timer timer;
        timer.start();
        ObjLoader loader(BenchConfig::kObjPath);
        loader.loadObj(scene);
        double ms = timer.stopMs();
        std::cout << "loadObj(" << BenchConfig::kObjPath << "): " << std::fixed
                  << std::setprecision(2) << ms << " ms\n";
        csvRows.push_back({"scene_load", 0, 0, ms, ms, ms, 0.0});
    }

    benchmarkRayGeneration(csvRows);
    benchmarkSimulate(scene, csvRows);

    writeCsv(BenchConfig::kCsvOut, csvRows);

    std::cout << "\nDone.\n";
    return 0;
}