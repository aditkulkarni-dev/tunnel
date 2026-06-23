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

Vector3D Vector3D::cross(const Vector3D &other) const
{
    return Vector3D({
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    });
}

Vector3D Vector3D::normalize() const
{
    float len = std::sqrt(x * x + y * y + z * z);

    if (len < 1e-6f)
        return Vector3D({0.0f, 0.0f, 0.0f});

    return Vector3D({
        x / len,
        y / len,
        z / len
    });
}

std::ostream &operator<<(std::ostream &os, const Vector3D &v)
{
    // TODO: insert return statement here
    os << "("
       << v.x << ", "
       << v.y << ", "
       << v.z << ")";

    return os;
}
