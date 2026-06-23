#include "RayUtils.h"
class Surface{
    protected:
        float absorptionCoefficient;
        
    
    public:
        Surface(float absorption) : absorptionCoefficient(absorption) {}
        virtual ~Surface() = default;

        virtual HitRecord calculateIntersection(const Ray& ray) const = 0;
        float getAbsorption() const {return absorptionCoefficient; }
        
};