#include "stdafx.h"

Vector3::Vector3()
	: x(0.0f), y(0.0f), z(0.0f) {
}

Vector3::Vector3(float scalar)
	: x(scalar), y(scalar), z(scalar) {
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z) {
}

Vector3::Vector3(const Vector2& other)
	: x(other.x), y(other.y), z(0.0f) {
}

Vector3::Vector3(float x, float y)
	: x(x), y(y), z(0.0f) {
}

Vector3::Vector3(const Vector4& other)
	: x(other.x), y(other.y), z(other.z) {
}

Vector3 Vector3::Up() {
	return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::Down() {
	return Vector3(0.0f, -1.0f, 0.0f);
}

Vector3 Vector3::Left() {
	return Vector3(-1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::Right() {
	return Vector3(1.0f, 1.0f, 0.0f);
}

Vector3 Vector3::Zero() {
	return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::XAxis() {
	return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::YAxis() {
	return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::ZAxis() {
	return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::Rotate(float angle, Vector3& axis) {
	Vector3 k = axis.Normalized();
	return *this * Math::Cos(angle) + k.Cross(y)* Math::Sin(angle) + (k * k.Dot(y)) * (1.0f - Math::Cos(angle));
}

Vector3& Vector3::Add(const Vector3& other) {
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

Vector3& Vector3::Subtract(const Vector3& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

Vector3& Vector3::Multiply(const Vector3& other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;

	return *this;
}

Vector3& Vector3::Divide(const Vector3& other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;

	return *this;
}

Vector3& Vector3::Add(float other) {
	x += other;
	y += other;
	z += other;

	return *this;
}

Vector3& Vector3::Subtract(float other) {
	x -= other;
	y -= other;
	z -= other;

	return *this;
}

Vector3& Vector3::Multiply(float other) {
	x *= other;
	y *= other;
	z *= other;

	return *this;
}

Vector3& Vector3::Divide(float other) {
	x /= other;
	y /= other;
	z /= other;

	return *this;
}

Vector3 Vector3::Multiply(const Matrix4& transform) const {
	return Vector3(
		transform.rows[0].x * x + transform.rows[0].y * y + transform.rows[0].z * z + transform.rows[0].w,
		transform.rows[1].x * x + transform.rows[1].y * y + transform.rows[1].z * z + transform.rows[1].w,
		transform.rows[2].x * x + transform.rows[2].y * y + transform.rows[2].z * z + transform.rows[2].w
	);
}

Vector3 operator+(Vector3 left, const Vector3& right) {
	return left.Add(right);
}

Vector3 operator-(Vector3 left, const Vector3& right) {
	return left.Subtract(right);
}

Vector3 operator*(Vector3 left, const Vector3& right) {
	return left.Multiply(right);
}

Vector3 operator/(Vector3 left, const Vector3& right) {
	return left.Divide(right);
}

Vector3 operator+(Vector3 left, float right) {
	return left.Add(right);
}

Vector3 operator-(Vector3 left, float right) {
	return left.Subtract(right);
}

Vector3 operator*(Vector3 left, float right) {
	return left.Multiply(right);
}

Vector3 operator/(Vector3 left, float right) {
	return left.Divide(right);
}

Vector3& Vector3::operator+=(const Vector3& other) {
	return Add(other);
}

Vector3& Vector3::operator-=(const Vector3& other) {
	return Subtract(other);
}

Vector3& Vector3::operator*=(const Vector3& other) {
	return Multiply(other);
}

Vector3& Vector3::operator/=(const Vector3& other) {
	return Divide(other);
}

Vector3& Vector3::operator+=(float other) {
	return Add(other);
}

Vector3& Vector3::operator-=(float other) {
	return Subtract(other);
}

Vector3& Vector3::operator*=(float other) {
	return Multiply(other);
}

Vector3& Vector3::operator/=(float other) {
	return Divide(other);
}

bool Vector3::operator<(const Vector3& other) const {
	return x < other.x && y < other.y && z < other.z;
}

bool Vector3::operator<=(const Vector3& other) const {
	return x <= other.x && y <= other.y && z <= other.z;
}

bool Vector3::operator>(const Vector3& other) const {
	return x > other.x && y > other.y && z > other.z;
}

bool Vector3::operator>=(const Vector3& other) const {
	return x >= other.x && y >= other.y && z >= other.z;
}

bool Vector3::operator==(const Vector3& other) const {
	return x == other.x && y == other.y && z == other.z;
}

bool Vector3::operator!=(const Vector3& other) const {
	return !(*this == other);
}

Vector3 operator-(const Vector3& vector) {
	return Vector3(-vector.x, -vector.y, -vector.z);
}

Vector3 Vector3::Cross(const Vector3& other) const {
	return Vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

float Vector3::Dot(const Vector3& other) const {
	return x * other.x + y * other.y + z * other.z;
}

float Vector3::Magnitude() const {
	return (float)sqrt(x * x + y * y + z * z);
}

Vector3& Vector3::Normalize() {
	float length = Magnitude();
	x /= length;
	y /= length;
	z /= length;
	return *this;
}

Vector3 Vector3::Normalized() const {
	float length = Magnitude();
	return Vector3(x / length, y / length, z / length);
}

float Vector3::Distance(const Vector3& other) const {
	float a = x - other.x;
	float b = y - other.y;
	float c = z - other.z;
	return (float)sqrt(a * a + b * b + c * c);
}

void Vector3::Lerp(const Vector3& to, float amount) {
	x = Math::Lerp(x, to.x, amount);
	y = Math::Lerp(y, to.y, amount);
	z = Math::Lerp(z, to.z, amount);
}