#pragma once
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/CSVWriter.h"
#include <iostream>

int main(){
    int N_ITERS{200};
    std::vector<std::pair<int, Vector3D>> hitPoints;

    Vector3D SRC_POS{0.0f, 0.0f, 0.0f};
    Scene scene;
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{1, 0, 0}, Vector3D{-1, 0, 0}, 0.5f, 2.0f, 2.0f));
    scene.addSurface(std::make_unique<PlaneWall>(
        Vector3D{-1, 0, 0}, Vector3D{1, 0, 0}, 0.5f, 2.0f, 2.0f));
 
    IRSimulationConfig config;
    config.sourcePosition = {0.0f, 0.0f, 0.0f};
    config.numBounces = 200;
 
    RoomIRSimulator simulator(scene, config);
 
    Vector3D origin{0, 0, 0};
    std::vector<Ray> rays{
        Ray(origin, {1, 0.1, 0})
        // Ray(origin, {0, 1, 0}),
        // Ray(origin, {-1, 0, 0}),
        // ...add more directions here to sample the room more densely
    };
 
    SparseIR sparseIR = simulator.simulate(rays, hitPoints);
 
    std::cout << "Sparse IR Delays:\n";
    for (size_t i = 0; i < sparseIR.delays.size() && i <= 10; ++i) {
        std::cout << sparseIR.delays[i] << " ";
    }
    std::cout << "\n";
 
    std::cout << "Sparse IR Gains:\n";
    for (size_t i = 0; i < sparseIR.gains.size() && i <= 10; ++i) {
        std::cout << sparseIR.gains[i] << " ";
    }
    std::cout << "\n";

    writePointsToCSV(hitPoints);
    return 0;
}