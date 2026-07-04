#include "RoomIRSimulator.h"

RoomIRSimulator::RoomIRSimulator(const Scene& scene, IRSimulationConfig config)
    : scene(scene), config(config) {}

SparseIR RoomIRSimulator::simulate(std::vector<Ray> rays) const {
    SparseIR sparseIR{};

    // All rays are assumed to share a listener origin, matching the
    // single-listener setup this was ported from.
    if (!rays.empty()) {
        addDirectPathIfVisible(rays.front().origin, sparseIR);
    }

    // Bounce-major: every ray takes step 0, then every ray takes step 1,
    // etc. `rays` is mutated in place across iterations (accumulated
    // distance, absorption history, current origin/direction all live on
    // each Ray between bounces) - same role your `i`/`r` loop variables
    // played in the original.
    for (int bounce = 0; bounce < config.numBounces; ++bounce) {
        for (auto& ray : rays) {
            advanceRay(ray, sparseIR);
        }
    }

    return sparseIR;
}

void RoomIRSimulator::addDirectPathIfVisible(const Vector3D& listenerPos, SparseIR& outIR) const {
    Vector3D dirToSrc = config.sourcePosition - listenerPos;
    float distanceToSrc = dirToSrc.length();
    Ray directRay(listenerPos, dirToSrc.normalize());

    if (scene.isOccluded(directRay, distanceToSrc)) {
        return;
    }

    float energy = 1.0f / (1.0f + distanceToSrc * distanceToSrc);
    outIR.gains.push_back(energy);
    outIR.delays.push_back(distanceToSrc);
    outIR.rays.push_back(directRay);
}

bool RoomIRSimulator::advanceRay(Ray& ray, SparseIR& outIR) const {
    HitRecord hit;
    Surface* hitSurface = nullptr;

    if (!scene.intersectClosest(ray, hit, &hitSurface)) {
        // Ray escaped the scene - nothing to bounce off of this step.
        return false;
    }

    ray.accumulatedDistance += hit.t;
    ray.absorptionHistory.push_back(hitSurface->getAbsorption());

    Vector3D shadowOrigin = hit.hitPoint + hit.surfaceNormal * config.shadowRayEpsilon;
    float distanceToSrc = 0.0f;

    if (nextEventEstimation(shadowOrigin, distanceToSrc)) {
        float totalPathDistance = ray.accumulatedDistance + distanceToSrc;
        float energy = computeEnergy(totalPathDistance, ray.absorptionHistory);

        outIR.gains.push_back(energy);
        outIR.delays.push_back(totalPathDistance);
        outIR.rays.push_back(ray);
    }

    ray.origin = shadowOrigin;
    ray.direction = reflect(ray.direction, hit.surfaceNormal);
    return true;
}

bool RoomIRSimulator::nextEventEstimation(const Vector3D& origin, float& outDistance) const {
    Vector3D dirToSrc = config.sourcePosition - origin;
    outDistance = dirToSrc.length();
    Ray shadowRay(origin, dirToSrc.normalize());

    return !scene.isOccluded(shadowRay, outDistance);
}

float RoomIRSimulator::computeEnergy(float totalDistance, const std::vector<float>& absorptionHistory) {
    float energy = 1.0f;
    for (float absorption : absorptionHistory) {
        energy *= (1.0f - absorption);
    }
    return energy / (1.0f + totalDistance * totalDistance);
}