#pragma once

struct Vector2;
struct Vector4;
struct Matrix4;

struct Vector3 {
	union {
		struct { float x, y, z; };
		struct { float pitch, yaw, roll; };
		float values[3];
	};

	Vector3();
	Vector3(float scalar);
	Vector3(float x, float y, float z);
	Vector3(const aiVector3D& vec) : x(vec.x), y(vec.y), z(vec.z) {}
	Vector3(const Vector2& other);
	Vector3(float x, float y);
	Vector3(const Vector4& other);

	static Vector3 Up();
	static Vector3 Down();
	static Vector3 Left();
	static Vector3 Right();
	static Vector3 Zero();

	static Vector3 XAxis();
	static Vector3 YAxis();
	static Vector3 ZAxis();

	Vector3 Rotate(float angle, Vector3& axis);

	Vector3& Add(const Vector3& other);
	Vector3& Subtract(const Vector3& other);
	Vector3& Multiply(const Vector3& other);
	Vector3& Divide(const Vector3& other);

	Vector3& Add(float other);
	Vector3& Subtract(float other);
	Vector3& Multiply(float other);
	Vector3& Divide(float other);

	Vector3 Multiply(const Matrix4& transform) const;

	friend Vector3 operator+(Vector3 left, const Vector3& right);
	friend Vector3 operator-(Vector3 left, const Vector3& right);
	friend Vector3 operator*(Vector3 left, const Vector3& right);
	friend Vector3 operator/(Vector3 left, const Vector3& right);

	friend Vector3 operator+(Vector3 left, float right);
	friend Vector3 operator-(Vector3 left, float right);
	friend Vector3 operator*(Vector3 left, float right);
	friend Vector3 operator/(Vector3 left, float right);

	bool operator==(const Vector3& other) const;
	bool operator!=(const Vector3& other) const;

	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);
	Vector3& operator*=(const Vector3& other);
	Vector3& operator/=(const Vector3& other);

	Vector3& operator+=(float other);
	Vector3& operator-=(float other);
	Vector3& operator*=(float other);
	Vector3& operator/=(float other);

	bool operator<(const Vector3& other) const;
	bool operator<=(const Vector3& other) const;
	bool operator>(const Vector3& other) const;
	bool operator>=(const Vector3& other) const;

	friend Vector3 operator-(const Vector3& vector);

	Vector3 Cross(const Vector3& other) const;
	float Dot(const Vector3& other) const;

	float Magnitude() const;
	Vector3& Normalize();
	Vector3 Normalized() const;
	float Distance(const Vector3& other) const;

	void Lerp(const Vector3& to, float amount);
};