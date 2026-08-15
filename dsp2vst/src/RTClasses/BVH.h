#include "Geometry.h"
#include "Surfaces.h"
#include <cfloat>
#include <algorithm>

struct BoundingBox{
    Vector3D min;
    Vector3D max;
    bool intersects(const Ray& ray) const{
        auto o = ray.origin;
        auto d = ray.direction;

        float t1 = (min.x - o.x)/d.x;
        float t2 = (max.x - o.x)/d.x;
        float txmin = std::min(t1, t2);
        float txmax = std::max(t1, t2);

        float ty1 = (min.y - o.y)/d.y;
        float ty2 = (max.y - o.y)/d.y;
        float tymin = std::min(ty1, ty2);
        float tymax = std::max(ty1, ty2);

        float tz1 = (min.z - o.z) / d.z;
        float tz2 = (max.z - o.z) / d.z;
        float tzmin = std::min(tz1, tz2);
        float tzmax = std::max(tz1, tz2);

        float tEnter = std::max(txmin, std::max(tymin, tzmin));
        float tExit = std::min(txmax, std::min(tymax, tzmax));


        return tExit >= tEnter && tExit >= 0.0f;
    };
     void expand(const Vector3D& p)
    {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);

        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }
};

struct BVHNode{
    BoundingBox bounds;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    std::vector<Triangle*> triangles;

    bool isLeaf() const{
        return (left == nullptr) && (right == nullptr);
    }
};

template <typename Iterator>
BoundingBox computeBounds(Iterator begin, Iterator end){
    BoundingBox box;

    box.min = {
        FLT_MAX,
        FLT_MAX,
        FLT_MAX
    };

    box.max = {
        -FLT_MAX,
        -FLT_MAX,
        -FLT_MAX
    };

     for(auto it = begin; it != end; ++it)
    {
        const Triangle& tri = static_cast<const Triangle&>(*(*it));

        box.expand(tri.getA());
        box.expand(tri.getB());
        box.expand(tri.getC());
    }
    return box;
}

inline int longestAxis(const BoundingBox& box){
    Vector3D diff = box.max - box.min;
    return (diff.x > diff.y) ? ((diff.x > diff.z) ? 0 : 2 ) : ((diff.y > diff.z)? 1 : 2);
}

template <typename Iterator>
std::unique_ptr<BVHNode> buildBVH(Iterator begin, Iterator end){
    auto node = std::make_unique<BVHNode>();
    node->bounds = computeBounds(begin, end);
    size_t count = end - begin;
    if (count <= 4){
        for(auto it = begin; it != end; ++it){
            node->triangles.push_back(*it);
        }
        
        return node;
    }
    int axis = longestAxis(node->bounds);
    auto mid = begin + count/2;
    std::nth_element(begin, mid, end, [axis](Triangle* a, Triangle* b){
        return a->centroid[axis] < b->centroid[axis];
    });
    node->left = buildBVH(begin, mid);
    node->right = buildBVH(mid, end);
    return node;
}
