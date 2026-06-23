#include "Surface.h"

// ax + by + cz + d = 0
class PlaneWall : public Surface{
private:
    // defines the surface
    Vector3D normal;
    Vector3D pointOnSurface;

    // constrains the surface
    Vector3D u;
    Vector3D v;
    
    float height;
    float width;
    float planeConstant;

    // for checking edge-cases
    static constexpr float EPSILON = 1e-6f;

public:
    PlaneWall( Vector3D pointOnSurface, Vector3D norm,
        float abs, float height, float width):
        Surface(abs), normal(norm), height(height), width(width),
        pointOnSurface(pointOnSurface)
        {
            
            normal = norm.normalize();
            planeConstant = normal.dot(this->pointOnSurface);
            Vector3D xAxis({1, 0, 0});
            Vector3D yAxis({0, 1, 0});

            Vector3D helper;

            if (std::abs(normal.dot(xAxis)) < 0.9f)
                helper = xAxis;
            else
                helper = yAxis;

            u = (normal.cross(helper).normalize());
            v = (normal.cross(u).normalize());
        }

        HitRecord calculateIntersection(const Ray& ray) const override;
        
};