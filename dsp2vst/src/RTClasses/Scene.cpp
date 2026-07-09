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

void writeHeatmapMTL(std::string filename)
{
    std::ofstream mtl(filename);

    mtl << "newmtl LowHits\n";
    mtl << "Kd 0.0 0.0 1.0\n\n";

    mtl << "newmtl MediumHits\n";
    mtl << "Kd 0.0 1.0 0.0\n\n";

    mtl << "newmtl HighHits\n";
    mtl << "Kd 1.0 0.0 0.0\n\n";
}

void writeAcousticHeatmapOBJ(Scene &scene, std::string obj_filename, std::string mtl_filename)
{
    uint64_t maxHits{0};
    writeHeatmapMTL(mtl_filename);

    std::ofstream obj(obj_filename);
    obj << "mtllib " << mtl_filename << "\n";

    uint64_t totalHits = 0;
    uint64_t trianglesHit = 0;

    for (const auto& surface : scene.getSurfaces()){
        // downcast surface* to a triangle*
        Triangle* tri = dynamic_cast<Triangle*>(surface.get());

        if (tri){
            totalHits += tri->getHitCount();
            if (tri->getHitCount() > 0){
                trianglesHit++;
            }
            maxHits = std::max(maxHits, tri->getHitCount());
        }
    }
    std::cout << "maxHits = " << maxHits << "\n";

    std::cout << "Total hits: " << totalHits << "\n";
    std::cout << "Triangles hit: " << trianglesHit << "\n";
    int vertexIdx{1};

    for (const auto& surface : scene.getSurfaces()){
        Triangle* tri = dynamic_cast<Triangle*>(surface.get());
        
        if (tri && maxHits > 0){
            Vector3D A = tri->getA();
            Vector3D B = tri->getB();
            Vector3D C = tri->getC();
            float normalizedHits =  static_cast<float>(tri->getHitCount()) / maxHits;

            
            obj << "v " << A.x << " " << A.y << " " << A.z << "\n";
            obj << "v " << B.x << " " << B.y << " " << B.z << "\n";
            obj << "v " << C.x << " " << C.y << " " << C.z << "\n";

            if(normalizedHits < 0.33f){
                obj << "usemtl LowHits" << "\n";
            }
            else if(normalizedHits < 0.66f){
                obj << "usemtl MediumHits" << "\n";
            }
            else{
                obj << "usemtl HighHits" << "\n";
            }
            obj << "f " << vertexIdx << " " << vertexIdx+1 << " " << vertexIdx+2 << "\n";
            vertexIdx+=3;

        }

    }
}
