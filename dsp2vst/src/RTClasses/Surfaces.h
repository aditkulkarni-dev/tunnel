#include "Surface.h"

// ax + by + cz + d = 0
class PlaneWall : Surface{
private:
    // defines the surface
    float d;
    Vector3D normal;
    
    // constrains the surface
    float minX, maxX, minY, maxY, minZ, maxZ;


    // for checking edge-cases
    static constexpr float EPSILON = 1e-6f;

public:
    PlaneWall( Vector3D norm, float d,
        float abs, float x0, float x1, float y0, float y1, float z0, float z1):
        Surface(abs), minX(x0), maxX(x1),
        minY(y0), maxY(y1), minZ(z0), maxZ(z1), d(d), normal(norm)
        {

        }

        HitRecord calculateIntersection(const Ray& ray) const override;
        
};