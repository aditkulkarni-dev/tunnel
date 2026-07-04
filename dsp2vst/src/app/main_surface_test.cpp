#pragma once
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/Scene.h"
#include "../RTClasses/RoomIRSimulator.h"
#include "../helpers/CSVWriter.h"
#include <iostream>

int main(){
    int N_ITERS{200};

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
        Ray(origin, {1, 0, 0})
        // Ray(origin, {0, 1, 0}),
        // Ray(origin, {-1, 0, 0}),
        // ...add more directions here to sample the room more densely
    };
 
    SparseIR sparseIR = simulator.simulate(rays);
 
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












    // SparseIR sparseIR{};

    // std::vector<PlaneWall> cube({
    //          //    center     normal    abs    h    w
    //     PlaneWall({1, 0, 0}, {-1, 0, 0}, 0.5f, 2.0f, 2.0f),
    //     PlaneWall({-1, 0, 0}, {1, 0, 0}, 0.5f, 2.0f, 2.0f)
    //     // PlaneWall({-1, 0, 0}, {-1, 0, 0}, 0.3, 2.0f, 2.0f),
    //     // PlaneWall({0, 1, 0}, {0, 1, 0}, 0.3, 2.0f, 2.0f),
    //     // PlaneWall({0, -1, 0}, {0, -1, 0}, 0.3, 2.0f, 2.0f),
    //     // PlaneWall({0, 0, 1}, {0, 0, 1}, 0.3, 2.0f, 2.0f),
    //     // PlaneWall({0, 0, -1}, {0, 0, -1}, 0.3, 2.0f, 2.0f),
    //     });

    //     Vector3D origin({0, 0, 0});
    //     std::vector<Ray> rays({
    //         // Ray({ origin, {1, 0, 0}}),
    //         // Ray({ origin, {0, 1, 0}}),
    //         // Ray({ origin, {-1, 0, 0}}),
    //         // Ray({ origin, {0, -1, 0}}),
    //         // Ray({ origin, {0, 0, 1}}),
    //         // Ray({ origin, {0, 0, -1}}),

    //          Ray(origin, {1, 0, 0})

    //     });

    // std::vector<std::pair<int, Vector3D>> hitPoints;

    // // Handle 0-reflection case

    // Vector3D dirToSrc = SRC_POS - origin;
    // float distanceToSrc = dirToSrc.length();
    // Vector3D directRayDir = dirToSrc.normalize();

    // Ray directRay(origin, directRayDir);
    // bool isSourceBlocked = false;

    // // Check if any walls are blocking the direct line of sight
    // for (const auto& surface : cube){
    //     HitRecord record = surface.calculateIntersection(directRay);
    //     if (record.didHit && record.t < distanceToSrc){
    //         isSourceBlocked = true;
    //         break; 
    //     }
    // }

    // if (!isSourceBlocked){
    //     float energy = 1.0f / (1.0f + distanceToSrc * distanceToSrc);
    //     sparseIR.gains.push_back(energy);
    //     sparseIR.delays.push_back(distanceToSrc);
    //     sparseIR.rays.push_back(directRay);
        
    //     std::cout << "[SUCCESS] Direct Path Logged! Delay: " << distanceToSrc 
    //               << " | Energy: " << energy << "\n\n";
    // }


    // for (int i{0}; i < N_ITERS; ++i){
    //     for (int r{0}; r < rays.size(); ++r){
    //         auto& ray = rays[r];

    //         float t_min = std::numeric_limits<float>::max();
    //         Surface* hitSurface{nullptr};
    //         HitRecord closestRecord;


    //         for (auto& surface : cube){
    //             HitRecord record = surface.calculateIntersection(ray);
    //             if (record.didHit && record.t < t_min){
                    
    //                 t_min = record.t;
    //                 hitSurface = &surface;
    //                 closestRecord = record;
    //             }
    //         }
    //         if (hitSurface){
    //             hitPoints.push_back({r, closestRecord.hitPoint}); 
    //             ray.accumulatedDistance += closestRecord.t;
    //             ray.absorptionHistory.push_back(hitSurface->getAbsorption());
    //             // NEXT EVENT ESTIMATION (SHADOW RAY)

    //             float epsilon{0.001f};

    //             Vector3D shadowRayOrigin = closestRecord.hitPoint + closestRecord.surfaceNormal * epsilon;
    //             Vector3D dirToSrc = SRC_POS - shadowRayOrigin;
    //             float distanceToSrc = dirToSrc.length();
    //             Vector3D shadowRayDir = dirToSrc.normalize();

    //             Ray shadowRay(shadowRayOrigin, shadowRayDir);
    //             bool isSourceBlocked = false;

    //             for (const auto& surface : cube){
    //                 HitRecord shadowRecord = surface.calculateIntersection(shadowRay);
    //                 if (shadowRecord.didHit && shadowRecord.t < distanceToSrc){
    //                 isSourceBlocked = true;
    //                 break; 
    //                 }
    //             }

    //             if (!isSourceBlocked){
    //                 float energy = 1.0f;
    //                 float totalPathDistance = ray.accumulatedDistance + distanceToSrc;
    //                 for (const auto& absorption : ray.absorptionHistory){
    //                     float reflectionCoef = 1.0f - absorption;
    //                     energy *= reflectionCoef;
    //                 }
    //                 energy = energy / (1.0f + totalPathDistance*totalPathDistance);
    //                 sparseIR.gains.push_back(energy);
    //                 sparseIR.delays.push_back(totalPathDistance);
    //                 sparseIR.rays.push_back(ray);
    //             }



    //             // std::cout << "Stored ray direction = " << ray.direction << "\n";
    //             ray.origin = closestRecord.hitPoint + closestRecord.surfaceNormal * epsilon;
    //             // std::cout << "Ray direction : " << ray.direction << "\n";
    //             // std::cout << "Surface direction : " << closestRecord.surfaceNormal << "\n";
    //             // std::cout << "time of intersection : " << closestRecord.t << "\n";
    //             // std::cout << "point of intersection : " << closestRecord.hitPoint << "\n";
    //             // std::cout << "Reflected = "<< reflect(ray.direction, closestRecord.surfaceNormal) << "\n";
    //             ray.direction = reflect(ray.direction, closestRecord.surfaceNormal);
    //         }
    //     }
    // }
    // std::cout << "Sparse IR Delays : \n";
    // int i{0};

    // for(const auto& delay : sparseIR.delays){
    //     if(i > 10){
    //         break;
    //     }
    //     std::cout << delay << " ";
    //     i++;
    // }
    // std::cout << "\n";

    // int j{0};
    // std::cout << "Sparse IR Gains : \n";
    // for(const auto& gain : sparseIR.gains){
    //     if(j > 10){
    //         break;
    //     }
    //     std::cout << gain << " ";
    //     j++;
    // }


    
    // PlaneWall wall(
    //     {0, 0, 0},
    //     {0, 0, 1},
    //     0.5f,
    //     10.0f,
    //     10.0f
    // );

    // Ray ray(
    //     {{2, 2, -5},
    //     {0, 0, 1}}
    // );



    // HitRecord record = wall.calculateIntersection(ray);
    // std::cout << "Did it hit? : " << record.didHit << "\n";
    // std::cout << "time of intersection : " << record.t << "\n";
    // std::cout << "point of intersection : " << record.hitPoint << "\n";
    
    // writePointsToCSV(hitPoints);
    return 0;
}