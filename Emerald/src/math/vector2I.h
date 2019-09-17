#pragma once

struct Vector2I {
    int x, y;

    Vector2I();
    Vector2I(int scalar);
    Vector2I(int x, int y);

    Vector2I& Add(const Vector2I& other);
    Vector2I& Subtract(const Vector2I& other);
    Vector2I& Multiply(const Vector2I& other);
    Vector2I& Divide(const Vector2I& other);

    Vector2I& Add(int value);
    Vector2I& Subtract(int value);
    Vector2I& Multiply(int value);
    Vector2I& Divide(int value);

    friend Vector2I operator+(Vector2I left, const Vector2I& right);
    friend Vector2I operator-(Vector2I left, const Vector2I& right);
    friend Vector2I operator*(Vector2I left, const Vector2I& right);
    friend Vector2I operator/(Vector2I left, const Vector2I& right);

    friend Vector2I operator+(Vector2I left, int value);
    friend Vector2I operator-(Vector2I left, int value);
    friend Vector2I operator*(Vector2I left, int value);
    friend Vector2I operator/(Vector2I left, int value);

    bool operator==(const Vector2I& other) const;
    bool operator!=(const Vector2I& other) const;

    Vector2I& operator+=(const Vector2I& other);
    Vector2I& operator-=(const Vector2I& other);
    Vector2I& operator*=(const Vector2I& other);
    Vector2I& operator/=(const Vector2I& other);

    Vector2I& operator+=(int value);
    Vector2I& operator-=(int value);
    Vector2I& operator*=(int value);
    Vector2I& operator/=(int value);

    bool operator<(const Vector2I& other) const;
    bool operator<=(const Vector2I& other) const;
    bool operator>(const Vector2I& other) const;
    bool operator>=(const Vector2I& other) const;

	float Distance(const Vector2I& other) const;
};