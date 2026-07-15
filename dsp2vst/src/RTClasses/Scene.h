#pragma once

#include <vector>
#include <memory>
#include "../RTClasses/Surfaces.h"
#include <fstream>
#include "BVH.h"

// Owns the set of surfaces that make up a room/environment and answers
// intersection queries against all of them. This decouples "what geometry
// exists" from "how rays bounce around in it" (that's RoomIRSimulator's job) -
// Scene has zero knowledge of acoustics, energy, or IRs.
class Scene {
public:
    Scene() = default;

    // Takes ownership of the surface. Works with any Surface subclass
    // (PlaneWall today, others later) so the simulator never has to change
    // when you add new geometry types.
    void addSurface(std::unique_ptr<Surface> surface);

    // Finds the nearest surface hit by `ray` across all surfaces in the scene.
    // Returns true and fills outRecord (and outSurface, if non-null) on a hit,
    // false if the ray misses everything.
    bool anyHitBVH(const BVHNode* root, const Ray& ray, float maxDistance) const;
    bool intersectClosest(const Ray& ray, HitRecord& outRecord, Surface** outSurface = nullptr) const;
    bool intersectBVH(const BVHNode* node, const Ray& ray, HitRecord& bestRecord, Surface** bestSurface) const;
    // Shadow-ray / occlusion test: true if something blocks `ray` before it
    // travels `maxDistance`. Doesn't care *which* surface blocked it, so it
    // can early-out on the first hit - cheaper than intersectClosest.
    bool isOccluded(const Ray& ray, float maxDistance) const;
    int getSurfaceCount(){return surfaces.size();}
    size_t surfaceCount() const { return surfaces.size(); }
    void buildbvh();

    const std::vector<std::unique_ptr<Surface>>& getSurfaces() const {
    return surfaces;
    }


private:
    std::vector<std::unique_ptr<Surface>> surfaces;
    std::unique_ptr<BVHNode> tree;
};

void writeAcousticHeatmapOBJ(Scene &scene, std::string obj_filename, std::string mtl_filename);