#pragma once
#include "./Geometry.h"

class Surface{
    protected:
        float absorptionCoefficient;
        
    
    public:
        Surface(float absorption) : absorptionCoefficient(absorption) {}
        virtual ~Surface() = default;

        virtual HitRecord calculateIntersection(const Ray& ray) const = 0;
        virtual float getAbsorption() const {return absorptionCoefficient; }
        
};