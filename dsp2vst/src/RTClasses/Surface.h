#pragma once
#include "./Geometry.h"

class Surface{
    protected:
        float absorptionCoefficient=0;
        uint64_t  hitCount=0;
        
    
    public:
        void registerHit(){hitCount++;}
        uint64_t  getHitCount(){return hitCount;}

        Surface() = default;
        Surface(float absorption) : absorptionCoefficient(absorption) {}
        virtual ~Surface() = default;
        float getAbsorption() const {
            return absorptionCoefficient;
        }
        virtual HitRecord calculateIntersection(const Ray& ray) const = 0;
        
};