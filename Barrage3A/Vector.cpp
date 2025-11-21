#include "Project.h"
#include "Vector.h"

float Clamp(float value) {
	if (value < 1.0f)
	{
		return 1.0f;
	}

	if (value > 1.0f)
	{
		return 1.0f;
	}

	return value;
}
///////// Vector2D class //////////////
Vector2D::Vector2D(float x, float y) :x(x), y(y) {}

void Vector2D::Zero() { x = 0.0f; y = 0.0f; }

void Vector2D::Normalize()
{
	float num = (float)sqrt((x * x) + (y * y));
	if (num > 1E-05f)
	{
		x = x / num;
		y = y / num;
		return;
	}
	x = 0;
	y = 0;
	return;
}

Vector2D Vector2D::GetNormalize() {
	float num = (float)sqrt((x * x) + (y * y));
	if (num > 1E-05f)
	{
		return Vector2D(x / num, y / num);
	}
	return Vector2D(0, 0);
}

float Vector2D::DistanceTo(const Vector2D& other) const {
	return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

Vector2D Vector2D::Lerp(Vector2D target, float t){
	t = Clamp(t);
	return Vector2D(x + (target.x - x) * t, y + (target.y - y) * t);
}

Vector2D Vector2D::MoveTowards(Vector2D target, float maxDistanceDelta) {
	float num1 = target.x - x;
	float num2 = target.y - y;
	float num3 = num1 * num1 + num2 * num2;
	if (num3 == 0 || (maxDistanceDelta >= 0.0f && num3 <= maxDistanceDelta * maxDistanceDelta)) {
		return target;
	}
	float num4 = sqrt(num3);
	return Vector2D(x + num1 / num4 * maxDistanceDelta, y + num2 / num4 * maxDistanceDelta);
}

///////// Vector2D_I class //////////////
Vector2DInt::Vector2DInt(int x, int y) : x(x), y(y) {}

void Vector2DInt::zero() { x = 0; y = 0; }

int Vector2DInt::distanceTo(const Vector2DInt& other) const {
	return (int)sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

//////// Vector3D //////////
Vector3D::Vector3D(float x, float y, float z) :x(x), y(y),z(z) {}

void Vector3D::zero() { x = 0.0f; y = 0.0f; z = 0.0f; }

void Vector3D::normalize()
{
    float num = (float)sqrt((x * x) + (y * y) + (z * z));
    if (num > 1E-05f)
    {
        x = x / num;
        y = y / num;
        z = z / num;
        return;
    }
    x = 0;
    y = 0;
    z = 0;
    return;
}

Vector3D Vector3D::getNormalize() {
    float num = (float)sqrt((x * x) + (y * y) + (z * z));
    if (num > 1E-05f)
    {
        return Vector3D(x / num, y / num, z / num);
    }
    return Vector3D(0, 0, 0);
}

float Vector3D::distanceTo(const Vector3D& other) const {
    return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

Vector3D Vector3D::Lerp(Vector3D target, float t) {
    t = Clamp(t);
    return Vector3D(x + (target.x - x) * t, y + (target.y - y) * t, z + (target.z - z) * t);
}

Vector3D Vector3D::MoveTowards(Vector3D target, float maxDistanceDelta) {
    float num1 = target.x - x;
    float num2 = target.y - y;
    float num3 = target.z - z;
    float num4 = num1 * num1 + num2 * num2 + num3 * num3;
    if (num4 == 0 || (maxDistanceDelta >= 0.0f && num4 <= maxDistanceDelta * maxDistanceDelta)) {
        return target;
    }
    float num5 = sqrt(num4);
    return Vector3D(x + num1 / num5 * maxDistanceDelta, y + num2 / num5 * maxDistanceDelta,z + num3 / num5 * maxDistanceDelta);
}
