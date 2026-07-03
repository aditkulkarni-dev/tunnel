// CSVWriter.h
#pragma once
#include <fstream>
#include <string>
#include "../RTClasses/Surfaces.h"

inline void writePointsToCSV(const std::vector<std::pair<int, Vector3D>>& points, const std::string& filename = "intersections.csv") {
    std::ofstream file(filename);
    file << "ray_id,x,y,z\n";
    for (const auto& [id, p] : points)
        file << id << "," << p.x << "," << p.y << "," << p.z << "\n";
}