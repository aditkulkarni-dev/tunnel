#pragma once
#include <iostream>
#include <vector>

struct Vector3D{
    float x,y,z;

    Vector3D() = default;

    Vector3D(float x, float y, float z)
        : x(x), y(y), z(z)
    {}
    
    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(const Vector3D& other) const;
    Vector3D operator*(float scalar) const;
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v);
    float dot(const Vector3D& other) const;
    Vector3D cross(const Vector3D& other) const;
    Vector3D normalize() const;
    float length() const;
    
};

struct Ray{
    Vector3D origin;
    Vector3D direction; // Must be normalized
    float accumulatedDistance;
    float energy;
    std::vector<float> absorptionHistory;
    int id;

    Ray(const Vector3D& o, const Vector3D& d)
        : accumulatedDistance(0.0f), origin(o), direction(d.normalize())
    {
        
    }

};

struct HitRecord{
    bool didHit;
    float t;
    Vector3D hitPoint;
    Vector3D surfaceNormal;
};