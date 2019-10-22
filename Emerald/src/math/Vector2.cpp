#include "stdafx.h"

Vector2::Vector2()
	: x(0.0f), y(0.0f) {
}

Vector2::Vector2(float scalar)
	: x(scalar), y(scalar) {
}

Vector2::Vector2(int scalar)
	: x((float)scalar), y((float)scalar) {
}

Vector2::Vector2(float x, float y)
	: x(x), y(y) {
}

Vector2::Vector2(int x, int y)
	: x((float)x), y((float)y) {
}

Vector2::Vector2(const Vector3& vector) {
	this->x = vector.x;
	this->y = vector.y;
}

Vector2& Vector2::Add(const Vector2& other) {
	x += other.x;
	y += other.y;
	return *this;
}

Vector2& Vector2::Add(const Vector2I& other) {
	x += other.x;
	y += other.y;
	return *this;
}

Vector2& Vector2::Subtract(const Vector2& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2& Vector2::Subtract(const Vector2I& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

Vector2& Vector2::Multiply(const Vector2& other) {
	x *= other.x;
	y *= other.y;
	return *this;
}

Vector2& Vector2::Multiply(const Vector2I& other) {
	x *= other.x;
	y *= other.y;
	return *this;
}

Vector2& Vector2::Divide(const Vector2& other) {
	x /= other.x;
	y /= other.y;
	return *this;
}

Vector2& Vector2::Divide(const Vector2I& other) {
	x /= other.x;
	y /= other.y;
	return *this;
}

Vector2& Vector2::Add(float value) {
	x += value;
	y += value;

	return *this;
}

Vector2& Vector2::Subtract(float value) {
	x -= value;
	y -= value;

	return *this;
}

Vector2& Vector2::Multiply(float value) {
	x *= value;
	y *= value;

	return *this;
}

Vector2& Vector2::Divide(float value) {
	x /= value;
	y /= value;

	return *this;
}

Vector2 operator+(Vector2 left, const Vector2& right) { return left.Add(right); }
Vector2 operator+(Vector2 left, const Vector2I& right) { return left.Add(right); }

Vector2 operator-(Vector2 left, const Vector2& right) { return left.Subtract(right); }
Vector2 operator-(Vector2 left, const Vector2I& right) { return left.Subtract(right); }

Vector2 operator*(Vector2 left, const Vector2& right) { return left.Multiply(right); }
Vector2 operator*(Vector2 left, const Vector2I& right) { return left.Multiply(right); }

Vector2 operator/(Vector2 left, const Vector2& right) { return left.Divide(right); }
Vector2 operator/(Vector2 left, const Vector2I& right) { return left.Divide(right); }

Vector2 operator+(Vector2 left, float value) { return Vector2(left.x + value, left.y + value); }

Vector2 operator-(Vector2 left, float value) { return Vector2(left.x - value, left.y - value); }

Vector2 operator*(Vector2 left, float value) { return Vector2(left.x * value, left.y * value); }

Vector2 operator/(Vector2 left, float value) { return Vector2(left.x / value, left.y / value); }

Vector2& Vector2::operator+=(const Vector2& other) {
return Add(other);
}

Vector2& Vector2::operator-=(const Vector2& other) {
	return Subtract(other);
}

Vector2& Vector2::operator*=(const Vector2& other) {
	return Multiply(other);
}

Vector2& Vector2::operator/=(const Vector2& other) {
	return Divide(other);
}

Vector2& Vector2::operator+=(float value) {
	return Add(value);
}

Vector2& Vector2::operator-=(float value) {
	return Subtract(value);
}

Vector2& Vector2::operator*=(float value) {
	return Multiply(value);
}

Vector2& Vector2::operator/=(float value) {
	return Divide(value);
}

bool Vector2::operator==(const Vector2& other) const {
	return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2& other) const {
	return !(*this == other);
}

bool Vector2::operator<(const Vector2& other) const {
	return x < other.x && y < other.y;
}

bool Vector2::operator<=(const Vector2& other) const {
	return x <= other.x && y <= other.y;
}

bool Vector2::operator>(const Vector2& other) const {
	return x > other.x && y > other.y;
}

bool Vector2::operator>=(const Vector2& other) const {
	return x >= other.x && y >= other.y;
}

float Vector2::Distance(const Vector2& other) const {
	float a = x - other.x;
	float b = y - other.y;
	return Math::Sqrt(a * a + b * b);
}

float Vector2::Dot(const Vector2& other) const {
	return x * other.x + y * other.y;
}

float Vector2::Magnitude() const {
	return Math::Sqrt(x * x + y * y);
}

Vector2& Vector2::Normalize() {
	float length = Magnitude();
	x /= length;
	y /= length;
	return *this;
}

Vector2 Vector2::Normalized() const {
	float length = Magnitude();
	return Vector2(x / length, y / length);
}