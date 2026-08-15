#include "../RTClasses/Geometry.h"
#include "../RTClasses/Surfaces.h"
#include <limits>

struct BoundingBox{
    Vector3D min;
    Vector3D max;
    BoundingBox(std::vector<Triangle*> triangles){
        float min_x = std::numeric_limits<float>::max();
        float min_y = std::numeric_limits<float>::max();
        float min_z = std::numeric_limits<float>::max();

        float max_x = std::numeric_limits<float>::lowest();
        float max_y = std::numeric_limits<float>::lowest();
        float max_z = std::numeric_limits<float>::lowest();
        for (auto& triangle_ptr : triangles){
            for (auto& vertex : triangle_ptr->getTriangle()){
                float vertex_x = vertex.x;
                float vertex_y = vertex.y;
                float vertex_z = vertex.z;
                if (vertex_x > max_x){
                    max_x = vertex_x;
                }
                if (vertex_y > max_y){
                    max_y = vertex_y;
                }
                if (vertex_z > max_z){
                    max_z = vertex_z;
                }
                if (vertex_x < min_x){
                    min_x = vertex_x;
                }
                if (vertex_y < min_y){
                    min_y = vertex_y;
                }
                if (vertex_z < min_z){
                    min_z = vertex_z;
                }

            }
        }
        min.x = min_x;
        min.y = min_y;
        min.z = min_z;
        max.x = max_x;
        max.y = max_y;
        max.z = max_z;

    }

    BoundingBox(const BoundingBox& leftBounds, const BoundingBox& rightBounds){
        min.x = std::min(leftBounds.min.x, rightBounds.min.x);
        min.y = std::min(leftBounds.min.y, rightBounds.min.y);
        min.z = std::min(leftBounds.min.z, rightBounds.min.z);

        max.x = std::max(leftBounds.max.x, rightBounds.max.x);
        max.y = std::max(leftBounds.max.y, rightBounds.max.y);
        max.z = std::max(leftBounds.max.z, rightBounds.max.z);

    }


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
    std::vector<Triangle*> triangles;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
    BVHNode() = default;
    
    // constructor to set bounds, left and right
    BVHNode(std::vector<Triangle*> triangles) : triangles(std::move(triangles)), bounds(this->triangles){
        
    }

    BVHNode(std::unique_ptr<BVHNode> left, std::unique_ptr<BVHNode> right):
    left(std::move(left)), right(std::move(right)), 
    bounds(this->left->bounds, this->right->bounds)
    {
  
    }

    
    

    bool isLeaf() const{
        return (left == nullptr) && (right == nullptr);
    }
};



std::vector<float> calculate_cw(std::vector<Triangle*> triangles){
    // compute axis of largest difference
    float max_x{std::numeric_limits<float>::lowest()};
    float x_diff{std::numeric_limits<float>::lowest()};
    float y_diff{std::numeric_limits<float>::lowest()};
    float z_diff{std::numeric_limits<float>::lowest()};
    float max_y{std::numeric_limits<float>::lowest()};
    float max_z{std::numeric_limits<float>::lowest()};
    float min_x{std::numeric_limits<float>::max()};
    float min_y{std::numeric_limits<float>::max()};
    float min_z{std::numeric_limits<float>::max()};
    std::vector<float*> diffVector{&x_diff, &y_diff, &z_diff};
    std::vector<float*> minVector{&min_x, &min_y, &min_z};

    for (auto& triangle_ptr : triangles){
        float centroid_x = triangle_ptr->centroid[0];
        float centroid_y = triangle_ptr->centroid[1];
        float centroid_z = triangle_ptr->centroid[2];

        if(centroid_x > max_x){
            max_x = centroid_x;
        }
        if(centroid_y > max_y){
            max_y = centroid_y;
        }
        if(centroid_z > max_z){
            max_z = centroid_z;
        }
        if(centroid_x < min_x){
            min_x = centroid_x;
        }
        if(centroid_y < min_y){
            min_y = centroid_y;
        }
        if(centroid_z < min_z){
            min_z = centroid_z;
        }
        
    }
    // axis of largest difference
    x_diff = max_x - min_x;
    y_diff = max_y - min_y;
    z_diff = max_z - min_z;
    float w = (x_diff > y_diff) ? ((x_diff > z_diff) ? (0) : (2)): ((y_diff > z_diff )? (1) : (2));

    // compute Cw
    float cw = 0.5 * (*diffVector[w]) +  (*minVector[w]);

    return std::vector<float>{w, cw};
}


std::unique_ptr<BVHNode> buildBVH(std::vector<Triangle*> triangles){

    if (triangles.empty()) {
        return nullptr;
    }

    if(triangles.size() == 1){
        return std::make_unique<BVHNode>(triangles);
    }

    std::vector<Triangle*> leftGroup{};
    std::vector<Triangle*> rightGroup{};
    std::vector<float> cw_object = calculate_cw(triangles);
    int w = static_cast<int>(cw_object[0]);
    float cw = cw_object[1];

    for (auto& triangle_ptr : triangles){
        if (triangle_ptr->centroid[w] < cw){
            leftGroup.push_back(triangle_ptr);
        }
        else{
            rightGroup.push_back(triangle_ptr);
        }
    }
    // Fallback: If all triangles fell onto one side of the split plane
    if (leftGroup.empty() || rightGroup.empty()) {
        
        auto mid = triangles.begin() + (triangles.size() / 2);
        leftGroup.assign(triangles.begin(), mid);
        rightGroup.assign(mid, triangles.end());
    }

    std::unique_ptr<BVHNode> leftNode = buildBVH(leftGroup);
    std::unique_ptr<BVHNode> rightNode = buildBVH(rightGroup);
    std::unique_ptr<BVHNode> currentNode = std::make_unique<BVHNode>(std::move(leftNode), std::move(rightNode));
    return currentNode;
}