#pragma once
#include <vector>
#include "../RTClasses/Geometry.h"
#include <random>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif  

void generateRandomVectors(std::vector<Ray>& rays, const Vector3D& origin, int resolution=16){
    std::random_device dev;
    std::mt19937 gen(dev());
    std::uniform_real_distribution<float> thetaDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> uDist(-1.0f, 1.0f);

    

    for (int i{0}; i < resolution; ++i){
        float theta = thetaDist(gen);
        float u = uDist(gen);
        float r = std::sqrt(1.0f - u * u);

        Vector3D dir{
            r * std::cos(theta),
            r * std::sin(theta),
            u
        };

        rays.emplace_back(origin, dir);

    }
}