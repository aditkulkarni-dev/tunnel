#pragma once

#include <vector>
#include "Scene.h"
#include "../RTClasses/Surfaces.h"
#include "./SparseIR.h"
#include <random>
#include <cmath>

// Everything that's constant across one simulation run: where the source
// sits, how many bounces to trace, the shadow-ray offset. Kept separate from
// RoomIRSimulator so you can re-run with a new config (or a new source
// position for a different listener) without touching the scene or rays.
struct IRSimulationConfig {
    Vector3D sourcePosition{0.0f, 0.0f, 0.0f};
    int numBounces = 200;
    float shadowRayEpsilon = 0.001f; // offset to avoid self-intersection on NEE rays
};

// Traces rays through a Scene and produces a SparseIR via next-event
// estimation (NEE): every time a ray hits a surface, a shadow ray is fired
// straight at the source; if it's unobstructed, that path's delay/gain gets
// logged. Also logs the direct (0-bounce) path if it's unobstructed.

class RoomIRSimulator {
public:
    RoomIRSimulator(Scene& scene, IRSimulationConfig config);

    // simulate looks something like this:
    // for t in threads:
    //     for ray in thread_rays:
    //          advance_ray(ray)

    // advance_ray function uses Bounding Volume Hierarchy to find closest intersection
    // in O(log(n)) worst-case. 
    SparseIR simulate(const std::vector<Ray>& rays, std::vector<std::pair<int, Vector3D>>& hitPoints, int threads=16) const;
    void applyBeta(SparseIR& sparseIR, int numBins, float fMin, float fMax) const;
    
private:
    // Direct line-of-sight contribution from source to listenerPos, logged
    // once before any bouncing happens.
    void addDirectPathIfVisible(const Vector3D& listenerPos, SparseIR& outIR, float initialamplitude) const;


    bool advanceRay(Ray& ray, SparseIR& outIR) const;

    // Fires a shadow ray from `origin` toward the source. Returns true and
    // fills outDistance with the source distance if the path is unobstructed.
    bool nextEventEstimation(const Vector3D& origin, float& outDistance) const;

    // Inverse-square falloff over the full path length, scaled by the
    // product of per-bounce reflection coefficients (1 - absorption).
    float computeamplitude(float totalDistance, const std::vector<float>& absorptionHistory, Ray& ray) const;
    float beta(int numBins, float distance, float fMin, float fMax) const;
    static void merge(const SparseIR& src, SparseIR& dst);
    Scene& scene;
    IRSimulationConfig config;
};