#pragma once
#include "./Geometry.h"

class Surface{
    protected:
        float absorptionCoefficient;
        
    
    public:
        Surface() = default;
        Surface(float absorption) : absorptionCoefficient(absorption) {}
        virtual ~Surface() = default;
        float getAbsorption() const {
            return absorptionCoefficient;
        }
        virtual HitRecord calculateIntersection(const Ray& ray) const = 0;
        
};