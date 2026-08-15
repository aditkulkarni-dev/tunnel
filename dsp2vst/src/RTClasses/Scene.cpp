#include "Scene.h"
#include <limits>

void Scene::addSurface(std::unique_ptr<Surface> surface) {
    surfaces.push_back(std::move(surface));
}

bool Scene::anyHitBVH(const BVHNode *root, const Ray &ray, float maxDistance) const
{
    if (!tree){return false;}
    if(!root){return false;}
    if (!root->bounds.intersects(ray)){
        return false;
    }
    if(root->isLeaf()){
        for(Triangle* triangle : root->triangles){
            HitRecord record = triangle->calculateIntersection(ray);
            // for a direct ray going to source, this probably wont work
            // we would need to know how much time it takes from source to listener
            // and if record.t < time from src to listener, return true
            if (record.didHit && record.t > 0.001f && record.t < maxDistance){
                return true;
                
            }
        }
        return false;
    }
    bool leftHit = anyHitBVH(root->left.get(), ray, maxDistance);
    if (leftHit){return true;}
    bool rightHit = anyHitBVH(root->right.get(), ray, maxDistance);
    if(rightHit){return true;}

    return false;
}

bool Scene::intersectClosest(const Ray &ray, HitRecord &outRecord, Surface **outSurface) const
{

    outRecord.didHit = false;
    outRecord.t = std::numeric_limits<float>::max();
    bool ifIntersects = intersectBVH(tree.get(), ray, outRecord, outSurface);

    return ifIntersects;
}

bool Scene::intersectBVH(const BVHNode *node, const Ray &ray, HitRecord &bestRecord, Surface **bestSurface) const
{
    if (!tree){return false;}
    
    if(!node->bounds.intersects(ray)){
        return false;
    }
    if(node->isLeaf()){
    bool hitSomething = false;

    for(Triangle* triangle : node->triangles)
    {
        HitRecord record =
            triangle->calculateIntersection(ray);

        if(record.didHit && record.t < bestRecord.t)
        {
            bestRecord = record;

            if(bestSurface)
                *bestSurface = triangle;

            hitSomething = true;
        }
    }
    return hitSomething;
    }

    bool hitLeft = intersectBVH(node->left.get(), ray, bestRecord, bestSurface);
    bool hitRight = intersectBVH(node->right.get(), ray, bestRecord, bestSurface);

    return hitLeft || hitRight;
}

bool Scene::isOccluded(const Ray& ray, float maxDistance) const {
    // for (const auto& surface : surfaces) {
    //     HitRecord record = surface->calculateIntersection(ray);
    //     if (record.didHit && record.t < maxDistance) {
    //         return true;
    //     }
    // }

    bool hit = anyHitBVH(tree.get(), ray, maxDistance);
    return hit;
}

void Scene::buildbvh()
{
    std::vector<Triangle*> triangles;

    for (auto& surface : surfaces)
    {
        triangles.push_back(
            static_cast<Triangle*>(surface.get())
        );
    }
    tree = buildBVH(triangles.begin(), triangles.end());
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
    float maxHitDensity{0.0f};
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
            float density = static_cast<float>(tri->getHitCount()) / std::max(tri->getArea(), 1e-6f);

            if (density > maxHitDensity)
            {
                maxHitDensity = density;
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
            // float normalizedHits =  static_cast<float>(tri->getHitCount()) / maxHits;
            float density = static_cast<float>(tri->getHitCount()) / std::max(tri->getArea(), 1e-6f);
            float normalized = density / maxHitDensity;
            obj << "v " << A.x << " " << A.y << " " << A.z << "\n";
            obj << "v " << B.x << " " << B.y << " " << B.z << "\n";
            obj << "v " << C.x << " " << C.y << " " << C.z << "\n";

            if(normalized < 0.33f){
                obj << "usemtl LowHits" << "\n";
            }
            else if(normalized < 0.66f){
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
