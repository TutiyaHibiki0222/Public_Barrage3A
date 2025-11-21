#pragma once
/*
	Vector.h
*/
#include "Mathf.h"
#include "Reflection.h"
#include <DxLib.h>
/// <summary>
/// float型の Vector2D
/// </summary>
class Vector2D {
public:
    float x, y;
    JSON(x, y)
public:
    Vector2D(float = 0, float = 0);
    void Zero();
    void Normalize();
    float Angle() const {
        return atan2f(y, x) * (180.0f / static_cast<float>(Mathf::PI));
    }
    Vector2D GetNormalize();
    float DistanceTo(const Vector2D& other) const;
    Vector2D Lerp(Vector2D, float);
    Vector2D MoveTowards(Vector2D, float);
    float Dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }
    float Down(const Vector2D& other) const {
        return x * other.x - y * other.y;
    }
    float Length() const {
        return sqrt(x * x + y * y);
    }
    float LengthSquared() const {
        return x * x + y * y;
    }
    Vector2D Rotated(float angleRad) const {
        float cosA = cosf(angleRad);
        float sinA = sinf(angleRad);
        return Vector2D(
            x * cosA - y * sinA,
            x * sinA + y * cosA
        );
    }
    // 単項演算子 - （ベクトルの反転）
    Vector2D operator-() const {
        return Vector2D(-x, -y);
    }
    // 単項演算子 - （ベクトルの反転）
    Vector2D operator+() const {
        return Vector2D(+x, +y);
    }
    // 演算子オーバーロード
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator-(float value) const {
        return Vector2D(x - value, y - value);
    }

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator+(float value) const {
        return Vector2D(x + value, y + value);
    }

    Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2D operator*(const Vector2D& other) const {
        return Vector2D(x * other.x, y * other.y);
    }

    Vector2D operator*(float value) const {
        return Vector2D(x * value, y * value);
    }

    Vector2D& operator*=(float value) {
        x *= value;
        y *= value;
        return *this;
    }

    Vector2D& operator*=(const Vector2D& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }

    Vector2D operator/(float value) const {
        if (value == 0.0f) return *this;
        return Vector2D(x / value, y / value);
    }

    // 比較演算子
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }

    // スカラー倍演算子
    Vector2D operator/(const Vector2D& other) const {
        return Vector2D(x / other.x, y / other.y);
    }


    std::string print() const {
        std::ostringstream oss;
        oss << "(" << x << ", " << y << ")";
        return oss.str();
    }

    // 出力演算子オーバーロード
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    operator VECTOR() const {
        return VGet(x, y, 0.0f);
    }

public:
    static Vector2D FromAngle(float angleDegrees) {
        float angleRadians = Mathf::DegToRad(angleDegrees);
        return Vector2D(std::cos(angleRadians), std::sin(angleRadians));
    }

    static Vector2D Lerp(const Vector2D& a, const Vector2D& b, float t) {
        return a + (b - a) * t;
    }
};

// グローバルスコープで定義
inline Vector2D operator*(float value, const Vector2D& vec) {
    return Vector2D(vec.x * value, vec.y * value);
}

/// <summary>
/// Int型の Vector2D_I
/// </summary>
class Vector2DInt {
public:
	int x, y;
	Vector2DInt(int = 0,int = 0);

	void zero();

	int distanceTo(const Vector2DInt& other) const;
};


/// <summary>
/// float型の Vector3D
/// </summary>
class Vector3D {
public:
    float x, y, z;
    JSON(x, y, z)
public:
    Vector3D(float = 0, float = 0, float = 0);
    void zero();
    void normalize();
    Vector3D getNormalize();
    float distanceTo(const Vector3D& other) const;
    Vector3D Lerp(Vector3D, float);
    Vector3D MoveTowards(Vector3D, float);
    float dot(const Vector3D& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    float down(const Vector3D& other) const {
        return x * other.x - y * other.y - z * other.z;
    }
    float length() const {
        return sqrt(x * x + y * y + z * z);
    }
    // 演算子オーバーロード.
    Vector3D  operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    Vector3D  operator-(float value) const {
        return Vector3D(x - value, y - value, z - value);
    }
    Vector3D  operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    Vector3D  operator+(float value) const {
        return Vector3D(x + value, y + value, z + value);
    }
    Vector3D& operator+=(const Vector3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    Vector3D  operator*(const Vector3D& other) const {
        return Vector3D(x * other.x, y * other.y);
    }
    Vector3D  operator*(float value) const {
        return Vector3D(x * value, y * value,z * value);
    }
    Vector3D& operator*=(float value) {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    Vector3D& operator*=(const Vector3D& other) {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    Vector3D  operator/(float value) const {
        if (value == 0.0f) return *this; // Avoid division by zero
        return Vector3D(x / value, y / value);
    }
    // 比較演算子.
    bool operator==(const Vector3D& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    bool operator!=(const Vector3D& other) const {
        return !(*this == other);
    }

    // DxLibのVECTOR型への暗黙変換
    operator VECTOR() const {
        return VGet(x, y, z);
    }
};


namespace std {
    template <>
    struct hash<Vector2D> {
        size_t operator()(const Vector2D& v) const {
            return hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
        }
    };
}


