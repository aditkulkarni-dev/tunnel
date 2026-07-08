#pragma once
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/CSVWriter.h"
#include "../helpers/render.h"
#include "../helpers/VectorGen.h"
#include "../RTClasses/ObjLoader.h"
#include <iostream>
#include <math.h>


int main(){
    std::vector<std::pair<int, Vector3D>> hitPoints;
    Scene scene;
    std::string path{"sponza_triangulated_no_cube.obj"};
    Ray ray(
    Vector3D(0,0,-10),
    Vector3D(0,0,1)
    );
    ObjLoader loader(path);
    loader.loadObj(scene);
    

    IRSimulationConfig config;
    config.sourcePosition = {3.0f, 0.0f, 1.5f};
    config.numBounces = 10;
 
    RoomIRSimulator simulator(scene, config);
 
    Vector3D listner{0, 0, 1.5};
    std::vector<Ray> rays{
     
    };

    generateRandomVectors(rays, listner, 1000);
 
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
    renderDenseIRFractional(sparseIR, 44100, "C:/Program Files/Image-Line/FL Studio 2025/Data/Patches/Impulses/sponza_2.wav");

    return 0;
}