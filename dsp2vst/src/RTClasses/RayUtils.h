#include <vector>
#include <ostream>

struct Vector3D{
    float x,y,z;
    Vector3D operator+(const Vector3D& other) const;
    Vector3D operator-(const Vector3D& other) const;
    Vector3D operator*(const Vector3D& other) const;
    Vector3D operator*(float scalar) const;
    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v);
    float dot(const Vector3D& other) const;
    Vector3D cross(const Vector3D& other) const;
    Vector3D normalize() const;

};

struct Ray{
    Vector3D origin;
    Vector3D direction; // Must be normalized
};

struct RayState{
    Ray ray;
    float pathLength;
    float energy;
    int reflections;
    std::vector<int> surfaceID;
};

struct HitRecord{
    bool didHit;
    float t;
    Vector3D hitPoint;
    Vector3D surfaceNormal;
};