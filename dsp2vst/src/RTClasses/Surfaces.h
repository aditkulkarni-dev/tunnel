#pragma once
#include "Surface.h"



class Triangle : public Surface{
private:
    
    Vector3D A;
    Vector3D B;
    Vector3D C;
    Vector3D e1;
    Vector3D e2;
    Vector3D N;
    float absorption;
    static constexpr float EPSILON = 1e-6f;


public:

    Triangle(Vector3D A, Vector3D B, Vector3D C, float absorption):
     Surface(absorption), e1(B-A), e2(C-A), absorption(absorption), A(A), B(B), C(C)
    {
        Vector3D e1xe2 = e1.cross(e2);
        float length = e1xe2.length();
        if(length > EPSILON){
            N = (e1xe2)*(1.0f/length);
        }
        
    }
    HitRecord calculateIntersection(const Ray& ray) const override;

};








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