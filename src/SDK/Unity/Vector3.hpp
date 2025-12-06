#pragma once

#include <cmath>

namespace UnityEngine {
    struct Vector3 {
        float x, y, z;

        Vector3()
            : x(0)
            , y(0)
            , z(0) {}
        Vector3(float x, float y, float z)
            : x(x)
            , y(y)
            , z(z) {}

        static Vector3 Zero() { return { 0, 0, 0 }; }
        static Vector3 One() { return { 1, 1, 1 }; }
        static Vector3 Up() { return { 0, 1, 0 }; }
        static Vector3 Down() { return { 0, -1, 0 }; }

        Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
        Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }
        Vector3 operator*(float scalar) const { return { x * scalar, y * scalar, z * scalar }; }
        Vector3 operator/(float scalar) const { return { x / scalar, y / scalar, z / scalar }; }

        float dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }

        float magnitude() const { return std::sqrt(x * x + y * y + z * z); }

        float distance(const Vector3& other) const { return (*this - other).magnitude(); }

        Vector3 normalized() const {
            float m = magnitude();
            if (m > 1e-5f) {
                return *this / m;
            }
            return Zero();
        }
    };
}
