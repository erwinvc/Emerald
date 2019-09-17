#include "stdafx.h"

Vector2I::Vector2I()
    : x(0), y(0) {
}

Vector2I::Vector2I(int scalar)
    : x(scalar), y(scalar) {
}

Vector2I::Vector2I(int x, int y)
    : x(x), y(y) {
}

Vector2I& Vector2I::Add(const Vector2I& other) {
    x += other.x;
    y += other.y;

    return *this;
}

Vector2I& Vector2I::Subtract(const Vector2I& other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

Vector2I& Vector2I::Multiply(const Vector2I& other) {
    x *= other.x;
    y *= other.y;

    return *this;
}

Vector2I& Vector2I::Divide(const Vector2I& other) {
    x /= other.x;
    y /= other.y;

    return *this;
}

Vector2I& Vector2I::Add(int value) {
    x += value;
    y += value;

    return *this;
}

Vector2I& Vector2I::Subtract(int value) {
    x -= value;
    y -= value;

    return *this;
}

Vector2I& Vector2I::Multiply(int value) {
    x *= value;
    y *= value;

    return *this;
}

Vector2I& Vector2I::Divide(int value) {
    x /= value;
    y /= value;

    return *this;
}

Vector2I operator+(Vector2I left, const Vector2I& right) {
    return left.Add(right);
}

Vector2I operator-(Vector2I left, const Vector2I& right) {
    return left.Subtract(right);
}

Vector2I operator*(Vector2I left, const Vector2I& right) {
    return left.Multiply(right);
}

Vector2I operator/(Vector2I left, const Vector2I& right) {
    return left.Divide(right);
}

Vector2I operator+(Vector2I left, int value) {
    return Vector2I(left.x + value, left.y + value);
}

Vector2I operator-(Vector2I left, int value) {
    return Vector2I(left.x - value, left.y - value);
}

Vector2I operator*(Vector2I left, int value) {
    return Vector2I(left.x * value, left.y * value);
}

Vector2I operator/(Vector2I left, int value) {
    return Vector2I(left.x / value, left.y / value);
}

Vector2I& Vector2I::operator+=(const Vector2I& other) {
    return Add(other);
}

Vector2I& Vector2I::operator-=(const Vector2I& other) {
    return Subtract(other);
}

Vector2I& Vector2I::operator*=(const Vector2I& other) {
    return Multiply(other);
}

Vector2I& Vector2I::operator/=(const Vector2I& other) {
    return Divide(other);
}

Vector2I& Vector2I::operator+=(int value) {
    return Add(value);
}

Vector2I& Vector2I::operator-=(int value) {
    return Subtract(value);
}

Vector2I& Vector2I::operator*=(int value) {
    return Multiply(value);
}

Vector2I& Vector2I::operator/=(int value) {
    return Divide(value);
}

bool Vector2I::operator==(const Vector2I& other) const {
    return x == other.x && y == other.y;
}

bool Vector2I::operator!=(const Vector2I& other) const {
    return !(*this == other);
}

bool Vector2I::operator<(const Vector2I& other) const {
    return x < other.x && y < other.y;
}

bool Vector2I::operator<=(const Vector2I& other) const {
    return x <= other.x && y <= other.y;
}

bool Vector2I::operator>(const Vector2I& other) const {
    return x > other.x && y > other.y;
}

bool Vector2I::operator>=(const Vector2I& other) const {
    return x >= other.x && y >= other.y;
}

float Vector2I::Distance(const Vector2I& other) const {
	float a = (float)x - (float)other.x;
	float b = (float)y - (float)other.y;
	return Math::Sqrt(a * a + b * b);
}