#include "RayUtils.h"

Vector3D Vector3D::operator*(const Vector3D &other) const
{
    return Vector3D();
}

float Vector3D::dot(const Vector3D &other) const
{
    return (this->x * other.x + this->y * other.y + this->z * other.z);
}
