#pragma once
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/CSVWriter.h"
#include "../helpers/render.h"
#include "../helpers/VectorGen.h"
#include <iostream>
#include <math.h>
#include <chrono>
#include <iomanip>


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
    generateRandomVectors(rays, origin, 25000);
    std::vector<int> num_threads{1,2,4,8,16};
    
    std::cout << std::left
          << std::setw(10) << "Threads"
          << std::setw(15) << "Time (ms)"
          << '\n';

    const int NUM_RUNS = 5;

    for (int num_t : num_threads){
        double total_ms = 0.0;
        for (int i = 0; i < NUM_RUNS; i++)
            {
                hitPoints.clear();
                auto start = std::chrono::high_resolution_clock::now();
                SparseIR sparseIR = simulator.simulate(rays, hitPoints, num_t);
                auto end = std::chrono::high_resolution_clock::now();
                total_ms += std::chrono::duration<double, std::milli>(end - start).count();
            }
        std::cout << num_t << " threads -> "
                << total_ms / NUM_RUNS
                << " ms\n";
        }

    return 0;
}