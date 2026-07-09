#include "RoomIRSimulator.h"
#include <thread>

RoomIRSimulator::RoomIRSimulator(const Scene& scene, IRSimulationConfig config)
    : scene(scene), config(config) {}

SparseIR RoomIRSimulator::simulate(const std::vector<Ray>& rays, std::vector<std::pair<int, Vector3D>>& hitPoints, int threads) const {
    
    SparseIR sparseIR{};
    if (rays.empty()) return sparseIR;
    std::vector<std::thread> workers;
    int THREADS = std::min(threads, (int)rays.size());
    int stride = (rays.size() + THREADS - 1) / THREADS;
    std::vector<SparseIR> threadIR(THREADS);
    std::vector<std::vector<std::pair<int, Vector3D>>> threadHits(THREADS);

    if (!rays.empty()) {
        addDirectPathIfVisible(rays.front().origin, sparseIR, 1.0f / rays.size());
    }

    for (int t{0}; t < THREADS; ++t){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        workers.emplace_back([&, t](){
            for(int r{t*stride}; r < (t+1)*stride && r < rays.size(); ++r){
                auto ray = rays[r];
                // implement random flip between + and - here for ray energy
                
                
                float sign = dist(gen) ? 1.0f : -1.0f;
                ray.energy = sign * std::sqrt(1.0f / rays.size());
                for (int bounce = 0; bounce < config.numBounces; ++bounce) {
                    advanceRay(ray, threadIR[t]);
                }
            }
        });
    }

    for (auto& w : workers){
        w.join();
    }

    for (int t = 0; t < THREADS; ++t) {
        merge(threadIR[t], sparseIR);
        // hitPoints.insert(hitPoints.end(),
        //                 threadHits[t].begin(),
        //                 threadHits[t].end());
    }
    
    return sparseIR;
}

void RoomIRSimulator::addDirectPathIfVisible(const Vector3D& listenerPos, SparseIR& outIR, float initialEnergy) const {
    Vector3D dirToSrc = config.sourcePosition - listenerPos;
    float distanceToSrc = dirToSrc.length();
    Ray directRay(listenerPos, dirToSrc.normalize());

    if (scene.isOccluded(directRay, distanceToSrc)) {
        return;
    }

    float energy = initialEnergy / (1.0f + distanceToSrc);
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
    float absorption = hitSurface->getAbsorption();
    ray.energy *= (1.0f - absorption);
    ray.absorptionHistory.push_back(hitSurface->getAbsorption());
    hitSurface->registerHit();
    Vector3D faceNormal = (ray.direction.dot(hit.surfaceNormal) < 0) ? hit.surfaceNormal : hit.surfaceNormal*-1;
    Vector3D shadowOrigin = hit.hitPoint + faceNormal * config.shadowRayEpsilon;
    
    float distanceToSrc = 0.0f;

    if (nextEventEstimation(shadowOrigin, distanceToSrc)) {
        float totalPathDistance = ray.accumulatedDistance + distanceToSrc;
        float energy = computeEnergy(totalPathDistance, ray.absorptionHistory, ray);

        if (energy > 1.0f || !std::isfinite(energy)) {
            std::cout << "Bad energy!\n";

            for (float a : ray.absorptionHistory){
                std::cout << a << " ";
        }
        std::cout << "\n";
    }   
        
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

float RoomIRSimulator::computeEnergy(float totalDistance, const std::vector<float>& absorptionHistory, Ray& ray) const{
    
    return ray.energy / std::max(totalDistance, 1.0f);
}

void RoomIRSimulator::merge(const SparseIR &src, SparseIR &dst)
{

    dst.gains.insert(dst.gains.end(),
                     src.gains.begin(),
                     src.gains.end());

    dst.delays.insert(dst.delays.end(),
                      src.delays.begin(),
                      src.delays.end());

    dst.rays.insert(dst.rays.end(),
                    src.rays.begin(),
                    src.rays.end());
}
