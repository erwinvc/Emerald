#pragma once

struct Matrix4 {
	union {
		// [row + col * 4]
		float elements[4 * 4];
		Vector4 rows[4];
	};

	Matrix4();
	Matrix4(float diagonal);
	Matrix4(float* elements);
	Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);

	static Matrix4 Identity();

	Matrix4& Multiply(const Matrix4& other);
	friend Matrix4 operator*(Matrix4 left, const Matrix4& right);
	Matrix4& operator*=(const Matrix4& other);

	Vector3 Multiply(const Vector3& other) const;
	friend Vector3 operator*(const Matrix4& left, const Vector3& right);

	Vector4 Multiply(const Vector4& other) const;
	friend Vector4 operator*(const Matrix4& left, const Vector4& right);

	Matrix4& Invert();

	Vector4 GetColumn(unsigned int index) const;
	void SetColumn(unsigned int index, const Vector4& column);
	inline Vector3 GetPosition() const;
	inline void SetPosition(const Vector3& position);

	static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
	static Matrix4 Perspective(float fov, float aspectRatio, float near, float far);
	static Matrix4 LookAt(const Vector3& camera, const Vector3& object, const Vector3& up);

	static Matrix4 Translate(const Vector3& translation);
	static Matrix4 Rotate(float angle, const Vector3& axis);
	static Matrix4 Rotate(float x, float y, float z, float w);

	static Matrix4 Scale(const Vector3& scale);
	static Matrix4 Invert(const Matrix4& matrix);

	static Matrix4 Transpose(const Matrix4& matrix);

	static Matrix4 Lerp(const Matrix4& from, const Matrix4& to, float amount);

	static Matrix4 TRS(const Vector3& position, const Vector3& rotation, const Vector3& scale);
};