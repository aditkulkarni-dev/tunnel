#include "RoomIRSimulator.h"
#include <thread>

RoomIRSimulator::RoomIRSimulator(Scene& scene, IRSimulationConfig config)
    : scene(scene), config(config) {}

SparseIR RoomIRSimulator::simulate(const std::vector<Ray>& rays, std::vector<std::pair<int, Vector3D>>& hitPoints, int threads) const {
    
    SparseIR sparseIR{};
    scene.buildbvh();
    if (rays.empty()) return sparseIR;
    std::vector<std::thread> workers;
    int THREADS = std::min(threads, (int)rays.size());
    int stride = (rays.size() + THREADS - 1) / THREADS;
    std::vector<SparseIR> threadIR(THREADS);
    std::vector<std::vector<std::pair<int, Vector3D>>> threadHits(THREADS);

    if (!rays.empty()) {
        addDirectPathIfVisible(rays.front().origin, sparseIR, std::sqrt(1.0f / rays.size()));
    }

    for (int t{0}; t < THREADS; ++t){
 
        workers.emplace_back([&, t,
        gen = std::mt19937(std::random_device{}()),
         dist = std::uniform_int_distribution<int>(0, 1)
        ]() mutable{
            for(int r{t*stride}; r < (t+1)*stride && r < rays.size(); ++r){
                auto ray = rays[r];
                // implement random flip between + and - here for ray amplitude
                
                
                float sign = 1.0f;
                ray.amplitude = sign * std::sqrt(1.0f / rays.size());
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
    applyBeta(sparseIR, 50, 0.0f, 20000.0f);
    return sparseIR;
}

void RoomIRSimulator::applyBeta(SparseIR& sparseIR, int numBins, float fMin, float fMax) const{
    for(int i{0}; i < sparseIR.delays.size(); ++i){
        sparseIR.gains[i] *= beta(numBins, sparseIR.delays[i], fMin, fMax);
    }
}

float RoomIRSimulator::beta(int numBins, float distance, float fMin, float fMax) const{
    float sum{0.0f};
    const double pi = std::acos(-1.0f);
    for(int i{0}; i < numBins; ++i){
        sum += std::cos((2*pi*distance/343.0f)*(fMin + ((float)i/(numBins-1))*(fMax-fMin)));
    }
    return sum/numBins;
}

void RoomIRSimulator::addDirectPathIfVisible(const Vector3D& listenerPos, SparseIR& outIR, float initialamplitude) const {
    Vector3D dirToSrc = config.sourcePosition - listenerPos;
    float distanceToSrc = dirToSrc.length();
    Ray directRay(listenerPos, dirToSrc.normalize());

    if (scene.isOccluded(directRay, distanceToSrc)) {
        return;
    }

    float amplitude = initialamplitude / (1.0f + distanceToSrc);
    outIR.gains.push_back(amplitude);
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
    ray.amplitude *= std::sqrt(1.0f - absorption);
    ray.absorptionHistory.push_back(hitSurface->getAbsorption());
    hitSurface->registerHit();
    Vector3D faceNormal = (ray.direction.dot(hit.surfaceNormal) < 0) ? hit.surfaceNormal : hit.surfaceNormal*-1;
    Vector3D shadowOrigin = hit.hitPoint + faceNormal * config.shadowRayEpsilon;
    
    float distanceToSrc = 0.0f;

    if (nextEventEstimation(shadowOrigin, distanceToSrc)) {
        float totalPathDistance = ray.accumulatedDistance + distanceToSrc;
        float amplitude = computeamplitude(totalPathDistance, ray.absorptionHistory, ray);

        if (amplitude > 1.0f || !std::isfinite(amplitude)) {
            std::cout << "Bad amplitude!\n";

            for (float a : ray.absorptionHistory){
                std::cout << a << " ";
        }
        std::cout << "\n";
    }   
        
        outIR.gains.push_back(amplitude);
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

float RoomIRSimulator::computeamplitude(float totalDistance, const std::vector<float>& absorptionHistory, Ray& ray) const{
    
    return ray.amplitude / std::max(totalDistance, 1.0f);
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
