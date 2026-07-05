#pragma once

#include <vector>
#include "Scene.h"
#include "../RTClasses/Surfaces.h"
#include "./SparseIR.h"

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
//
// This is the reusable core: build a Scene once, pick a config, feed it a
// batch of rays (e.g. a sphere of directions from a listener position), get
// back a SparseIR you can hand straight to a convolution engine.
class RoomIRSimulator {
public:
    RoomIRSimulator(const Scene& scene, IRSimulationConfig config);

    // Advances every ray by one bounce, config.numBounces times, in lockstep
    // (bounce 0 for all rays, then bounce 1 for all rays, ...) rather than
    // finishing one ray's whole path before starting the next. This is
    // deliberate, not cosmetic: it's the loop order that lets a later,
    // time-varying direction D(t) or a shared field that rays read/write
    // stay consistent across rays at each timestep. Accumulates all
    // NEE-visible paths (plus the direct path, if unobstructed) into a
    // single SparseIR.
    SparseIR simulate(std::vector<Ray> rays, std::vector<std::pair<int, Vector3D>>& hitPoints, int threads=16) const;

private:
    // Direct line-of-sight contribution from source to listenerPos, logged
    // once before any bouncing happens.
    void addDirectPathIfVisible(const Vector3D& listenerPos, SparseIR& outIR) const;

    // Advances a single ray by exactly one bounce: finds the closest hit,
    // fires a shadow ray toward the source (logging a path if visible),
    // then reflects the ray off the surface it hit. Mutates `ray` in place.
    // Returns false if the ray hit nothing (escaped the scene), in which
    // case the caller should stop advancing it.
    bool advanceRay(Ray& ray, SparseIR& outIR) const;

    // Fires a shadow ray from `origin` toward the source. Returns true and
    // fills outDistance with the source distance if the path is unobstructed.
    bool nextEventEstimation(const Vector3D& origin, float& outDistance) const;

    // Inverse-square falloff over the full path length, scaled by the
    // product of per-bounce reflection coefficients (1 - absorption).
    static float computeEnergy(float totalDistance, const std::vector<float>& absorptionHistory);

    static void merge(const SparseIR& src, SparseIR& dst);
    const Scene& scene;
    IRSimulationConfig config;
};