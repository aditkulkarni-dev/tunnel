#include "RayUtils.h"

Vector3D Vector3D::operator+(const Vector3D &other) const
{
    return Vector3D({this->x + other.x, this->y + other.y, this->z + other.z});
}

Vector3D Vector3D::operator-(const Vector3D &other) const
{
    return Vector3D({this->x - other.x, this->y - other.y, this->z - other.z});
}

Vector3D Vector3D::operator*(const Vector3D &other) const
{
    return Vector3D();
}

Vector3D Vector3D::operator*(float scalar) const
{

    return Vector3D({scalar*this->x, scalar*this->y, scalar*this->z});
}

float Vector3D::dot(const Vector3D &other) const
{
    return (this->x * other.x + this->y * other.y + this->z * other.z);
}
