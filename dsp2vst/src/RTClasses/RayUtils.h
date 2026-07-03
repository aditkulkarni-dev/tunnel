#pragma once
#include <vector>
#include <ostream>

struct Vector3D{
    float x,y,z;
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
    std::vector<float> absorptionHistory;

    Ray(const Vector3D& o, const Vector3D& d)
        : accumulatedDistance(0.0f), origin(o), direction(d.normalize())
    {
        
    }

};

struct RayState{
    Ray ray;
    float pathLength;
    float energy;
    int reflections;
    std::vector<int> surfaceID;
};

struct HitRecord{
    bool didHit;
    float t;
    Vector3D hitPoint;
    Vector3D surfaceNormal;
};

struct SparseIR{
    std::vector<float> gains;
    std::vector<float> delays;
    std::vector<Ray> rays;

    int getSampleIndexInt(size_t pathIdx, float sampleRate = 44100.0f) const {
        if (pathIdx >= delays.size()) return -1;
        
        float speedOfSound = 343.0f;
        // If your delays vector stores totalPathDistance in meters:
        float timeInSeconds = delays[pathIdx] / speedOfSound; 
        
        return static_cast<int>(timeInSeconds * sampleRate);
    }

    float getSampleIndexFloat(size_t pathIdx, float sampleRate = 44100.0f) const {
        if (pathIdx >= delays.size()) return -1;
        
        float speedOfSound = 343.0f;
        // If your delays vector stores totalPathDistance in meters:
        float timeInSeconds = delays[pathIdx] / speedOfSound; 
        
        return (timeInSeconds * sampleRate);
    }
};

Vector3D reflect(Vector3D ray_direction, Vector3D surface_normal);
