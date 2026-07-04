// CSVWriter.h
#pragma once
#include <fstream>
#include <string>
#include "../RTClasses/Surfaces.h"
#include "../RTClasses/SparseIR.h"

inline void writePointsToCSV(const std::vector<std::pair<int, Vector3D>>& points, const std::string& filename = "intersections.csv") {
    std::ofstream file(filename);
    file << "ray_id,x,y,z\n";
    for (const auto& [id, p] : points)
        file << id << "," << p.x << "," << p.y << "," << p.z << "\n";
}

inline void writeIRToCSV(const SparseIR& sparseIR,
                         const std::string& filename = "ir_params.csv")
{
    std::ofstream file(filename);

    if (!file)
        throw std::runtime_error("Could not open " + filename);

    file << "delay,gain\n";

    for (size_t i = 0; i < sparseIR.gains.size(); ++i)
    {
        file << sparseIR.delays[i]
             << ','
             << sparseIR.gains[i]
             << '\n';
    }
}