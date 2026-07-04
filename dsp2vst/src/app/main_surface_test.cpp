#pragma once
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/CSVWriter.h"
#include "../helpers/render.h"
#include <iostream>
#include <math.h>
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

int main(){
    std::vector<std::pair<int, Vector3D>> hitPoints;

    Vector3D SRC_POS{0.0f, 0.0f, 0.0f};
    Scene scene;
    // Left / Right
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{15,0,0}, Vector3D{-1,0,0}, 0.0f, 20.0f, 10.0f));
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{-15,0,0}, Vector3D{1,0,0}, 0.0f, 20.0f, 10.0f));

    // Front / Back
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{0,20,0}, Vector3D{0,-1,0}, 0.0f, 30.0f, 10.0f));
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{0,-20,0}, Vector3D{0,1,0}, 0.0f, 30.0f, 10.0f));

    // Ceiling / Floor
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{0,0,8}, Vector3D{0,0,-1}, 0.0f, 30.0f, 40.0f));
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{0,0,-2}, Vector3D{0,0,1}, 0.0f, 30.0f, 40.0f));

    scene.addSurface(std::make_unique<PlaneWall>(
    Vector3D{0,5,0},
    Vector3D{1,-1,0}.normalize(),
    0.0f,
    6.0f,
    6.0f));

    scene.addSurface(std::make_unique<PlaneWall>(
    Vector3D{4,-3,2},
    Vector3D{-1,1,0}.normalize(),
    0.0f,
    5.0f,
    4.0f));


    
    IRSimulationConfig config;
    config.sourcePosition = {-8, -10, 1.5f};
    config.numBounces = 200;
 
    RoomIRSimulator simulator(scene, config);
 
    Vector3D origin{0, 0, 0};
    std::vector<Ray> rays{
     
    };
   
    generateRandomVectors(rays, origin, 500000);
 
    SparseIR sparseIR = simulator.simulate(rays, hitPoints);
 
    std::cout << "Sparse IR Delays:\n";
    for (size_t i = 0; i < sparseIR.delays.size() && i <= 10; ++i) {
        std::cout << sparseIR.delays[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Delays size : " << sparseIR.delays.size();
    std::cout << "\n";
 
    std::cout << "Sparse IR Gains:\n";
    for (size_t i = 0; i < sparseIR.gains.size() && i <= 10; ++i) {
        std::cout << sparseIR.gains[i] << " ";
    }
    std::cout << "\n";
    std::cout << "Gains size : " << sparseIR.gains.size();
    std::cout << "\n";

    writePointsToCSV(hitPoints);
    writeIRToCSV(sparseIR);
    renderDenseIRFractional(sparseIR, 44100, "C:/Program Files/Image-Line/FL Studio 2025/Data/Patches/Impulses/ir_test_inverse_decay.wav");
    return 0;
}