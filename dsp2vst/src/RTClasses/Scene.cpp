#include "Scene.h"
#include <limits>

void Scene::addSurface(std::unique_ptr<Surface> surface) {
    surfaces.push_back(std::move(surface));
}

bool Scene::intersectClosest(const Ray& ray, HitRecord& outRecord, Surface** outSurface) const {
    float tMin = std::numeric_limits<float>::max();
    Surface* hitSurface = nullptr;
    HitRecord closest;

    for (const auto& surface : surfaces) {
        HitRecord record = surface->calculateIntersection(ray);
        if (record.didHit && record.t < tMin) {
            tMin = record.t;
            hitSurface = surface.get();
            closest = record;
        }
    }

    if (!hitSurface) {
        return false;
    }

    outRecord = closest;
    if (outSurface) {
        *outSurface = hitSurface;
    }
    return true;
}

bool Scene::isOccluded(const Ray& ray, float maxDistance) const {
    for (const auto& surface : surfaces) {
        HitRecord record = surface->calculateIntersection(ray);
        if (record.didHit && record.t < maxDistance) {
            return true;
        }
    }
    return false;
}