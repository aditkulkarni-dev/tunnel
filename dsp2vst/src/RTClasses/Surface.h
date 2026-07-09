#pragma once
#include "./Geometry.h"

class Surface{
    protected:
        float absorptionCoefficient=0;
        std::atomic<uint64_t> hitCount=0;
        
    
    public:
        void registerHit(){
            hitCount.fetch_add(1, std::memory_order_relaxed);
        }
        uint64_t  getHitCount() const{
            return hitCount.load(std::memory_order_relaxed);
        }

        Surface() = default;
        Surface(float absorption) : absorptionCoefficient(absorption) {}
        virtual ~Surface() = default;
        float getAbsorption() const {
            return absorptionCoefficient;
        }
        virtual HitRecord calculateIntersection(const Ray& ray) const = 0;
        
};