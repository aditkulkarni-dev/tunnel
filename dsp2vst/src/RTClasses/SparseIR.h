#pragma once
#include <vector>
#include <ostream>
#include <limits>
#include "./Surface.h"
#include "./Geometry.h"

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

inline Vector3D reflect(Vector3D ray_direction, Vector3D surface_normal)
    {
        Vector3D d = ray_direction.normalize();
        Vector3D n = surface_normal.normalize();
        return (d - n * d.dot(n) * 2);
    }


