#include "stdafx.h"

Matrix4::Matrix4() {
	memset(elements, 0, 4 * 4 * sizeof(float));
}

Matrix4::Matrix4(float diagonal) {
	memset(elements, 0, 4 * 4 * sizeof(float));
	elements[0 + 0 * 4] = diagonal;
	elements[1 + 1 * 4] = diagonal;
	elements[2 + 2 * 4] = diagonal;
	elements[3 + 3 * 4] = diagonal;
}

Matrix4::Matrix4(float* elements) {
	memcpy(this->elements, elements, 4 * 4 * sizeof(float));
}

Matrix4::Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) {
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
	rows[3] = row3;
}

Matrix4 Matrix4::Identity() {
	return Matrix4(1.0f);
}

Matrix4& Matrix4::Multiply(const Matrix4& other) {
	float data[16];
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			float sum = 0.0f;
			for (int e = 0; e < 4; e++) {
				sum += elements[e + row * 4] * other.elements[col + e * 4];
			}
			data[col + row * 4] = sum;
		}
	}
	memcpy(elements, data, 4 * 4 * sizeof(float));
	return *this;
}

Vector3 Matrix4::Multiply(const Vector3& other) const {
	return other.Multiply(*this);
}

Vector4 Matrix4::Multiply(const Vector4& other) const {
	return other.Multiply(*this);
}

Matrix4 operator*(Matrix4 left, const Matrix4& right) {
	return left.Multiply(right);
}

Matrix4& Matrix4::operator*=(const Matrix4& other) {
	return Multiply(other);
}

Vector3 operator*(const Matrix4& left, const Vector3& right) {
	return left.Multiply(right);
}

Vector4 operator*(const Matrix4& left, const Vector4& right) {
	return left.Multiply(right);
}

Matrix4& Matrix4::Invert() {
	float temp[16];

	temp[0] = elements[5] * elements[10] * elements[15] -
		elements[5] * elements[11] * elements[14] -
		elements[9] * elements[6] * elements[15] +
		elements[9] * elements[7] * elements[14] +
		elements[13] * elements[6] * elements[11] -
		elements[13] * elements[7] * elements[10];

	temp[4] = -elements[4] * elements[10] * elements[15] +
		elements[4] * elements[11] * elements[14] +
		elements[8] * elements[6] * elements[15] -
		elements[8] * elements[7] * elements[14] -
		elements[12] * elements[6] * elements[11] +
		elements[12] * elements[7] * elements[10];

	temp[8] = elements[4] * elements[9] * elements[15] -
		elements[4] * elements[11] * elements[13] -
		elements[8] * elements[5] * elements[15] +
		elements[8] * elements[7] * elements[13] +
		elements[12] * elements[5] * elements[11] -
		elements[12] * elements[7] * elements[9];

	temp[12] = -elements[4] * elements[9] * elements[14] +
		elements[4] * elements[10] * elements[13] +
		elements[8] * elements[5] * elements[14] -
		elements[8] * elements[6] * elements[13] -
		elements[12] * elements[5] * elements[10] +
		elements[12] * elements[6] * elements[9];

	temp[1] = -elements[1] * elements[10] * elements[15] +
		elements[1] * elements[11] * elements[14] +
		elements[9] * elements[2] * elements[15] -
		elements[9] * elements[3] * elements[14] -
		elements[13] * elements[2] * elements[11] +
		elements[13] * elements[3] * elements[10];

	temp[5] = elements[0] * elements[10] * elements[15] -
		elements[0] * elements[11] * elements[14] -
		elements[8] * elements[2] * elements[15] +
		elements[8] * elements[3] * elements[14] +
		elements[12] * elements[2] * elements[11] -
		elements[12] * elements[3] * elements[10];

	temp[9] = -elements[0] * elements[9] * elements[15] +
		elements[0] * elements[11] * elements[13] +
		elements[8] * elements[1] * elements[15] -
		elements[8] * elements[3] * elements[13] -
		elements[12] * elements[1] * elements[11] +
		elements[12] * elements[3] * elements[9];

	temp[13] = elements[0] * elements[9] * elements[14] -
		elements[0] * elements[10] * elements[13] -
		elements[8] * elements[1] * elements[14] +
		elements[8] * elements[2] * elements[13] +
		elements[12] * elements[1] * elements[10] -
		elements[12] * elements[2] * elements[9];

	temp[2] = elements[1] * elements[6] * elements[15] -
		elements[1] * elements[7] * elements[14] -
		elements[5] * elements[2] * elements[15] +
		elements[5] * elements[3] * elements[14] +
		elements[13] * elements[2] * elements[7] -
		elements[13] * elements[3] * elements[6];

	temp[6] = -elements[0] * elements[6] * elements[15] +
		elements[0] * elements[7] * elements[14] +
		elements[4] * elements[2] * elements[15] -
		elements[4] * elements[3] * elements[14] -
		elements[12] * elements[2] * elements[7] +
		elements[12] * elements[3] * elements[6];

	temp[10] = elements[0] * elements[5] * elements[15] -
		elements[0] * elements[7] * elements[13] -
		elements[4] * elements[1] * elements[15] +
		elements[4] * elements[3] * elements[13] +
		elements[12] * elements[1] * elements[7] -
		elements[12] * elements[3] * elements[5];

	temp[14] = -elements[0] * elements[5] * elements[14] +
		elements[0] * elements[6] * elements[13] +
		elements[4] * elements[1] * elements[14] -
		elements[4] * elements[2] * elements[13] -
		elements[12] * elements[1] * elements[6] +
		elements[12] * elements[2] * elements[5];

	temp[3] = -elements[1] * elements[6] * elements[11] +
		elements[1] * elements[7] * elements[10] +
		elements[5] * elements[2] * elements[11] -
		elements[5] * elements[3] * elements[10] -
		elements[9] * elements[2] * elements[7] +
		elements[9] * elements[3] * elements[6];

	temp[7] = elements[0] * elements[6] * elements[11] -
		elements[0] * elements[7] * elements[10] -
		elements[4] * elements[2] * elements[11] +
		elements[4] * elements[3] * elements[10] +
		elements[8] * elements[2] * elements[7] -
		elements[8] * elements[3] * elements[6];

	temp[11] = -elements[0] * elements[5] * elements[11] +
		elements[0] * elements[7] * elements[9] +
		elements[4] * elements[1] * elements[11] -
		elements[4] * elements[3] * elements[9] -
		elements[8] * elements[1] * elements[7] +
		elements[8] * elements[3] * elements[5];

	temp[15] = elements[0] * elements[5] * elements[10] -
		elements[0] * elements[6] * elements[9] -
		elements[4] * elements[1] * elements[10] +
		elements[4] * elements[2] * elements[9] +
		elements[8] * elements[1] * elements[6] -
		elements[8] * elements[2] * elements[5];

	float determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
	determinant = 1.0f / determinant;

	for (int i = 0; i < 4 * 4; i++)
		elements[i] = temp[i] * determinant;

	return *this;
}

Vector4 Matrix4::GetColumn(unsigned int index) const {
	return Vector4(elements[index + 0 * 4], elements[index + 1 * 4], elements[index + 2 * 4], elements[index + 3 * 4]);
}

void Matrix4::SetColumn(unsigned int index, const Vector4& column) {
	elements[index + 0 * 4] = column.x;
	elements[index + 1 * 4] = column.y;
	elements[index + 2 * 4] = column.z;
	elements[index + 3 * 4] = column.w;
}

Vector3 Matrix4::GetPosition() const { return Vector3(GetColumn(3)); }
void Matrix4::SetPosition(const Vector3& position) { SetColumn(3, Vector4(position, 1.0f)); }

Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float nearr, float farr) {
	Matrix4 result(1.0f);

	result.elements[0 + 0 * 4] = 2.0f / (right - left);

	result.elements[1 + 1 * 4] = 2.0f / (top - bottom);

	result.elements[2 + 2 * 4] = 2.0f / (near - farr);

	result.elements[3 + 0 * 4] = (left + right) / (left - right);
	result.elements[3 + 1 * 4] = (bottom + top) / (bottom - top);
	result.elements[3 + 2 * 4] = (farr + nearr) / (farr - nearr);

	return result;
}

Matrix4 Matrix4::Perspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
	Matrix4 result(1.0f);

	float q = 1.0f / Math::Tan(Math::ToRadians(0.5f * fov));
	float a = q / aspectRatio;

	float b = (nearPlane + farPlane) / (nearPlane - farPlane);
	float c = (2.0f * nearPlane * farPlane) / (nearPlane - farPlane);

	result.elements[0 + 0 * 4] = a;
	result.elements[1 + 1 * 4] = q;
	result.elements[2 + 2 * 4] = b;
	result.elements[2 + 3 * 4] = -1.0f;
	result.elements[3 + 2 * 4] = c;

	return result;
}

Matrix4 Matrix4::LookAt(const Vector3& camera, const Vector3& object, const Vector3& up) {
	Matrix4 result = Identity();
	Vector3 f = (object - camera).Normalize();
	Vector3 s = f.Cross(up.Normalized());
	Vector3 u = s.Cross(f);

	result.elements[0 + 0 * 4] = s.x;
	result.elements[0 + 1 * 4] = s.y;
	result.elements[0 + 2 * 4] = s.z;

	result.elements[1 + 0 * 4] = u.x;
	result.elements[1 + 1 * 4] = u.y;
	result.elements[1 + 2 * 4] = u.z;

	result.elements[2 + 0 * 4] = -f.x;
	result.elements[2 + 1 * 4] = -f.y;
	result.elements[2 + 2 * 4] = -f.z;

	return result * Translate(Vector3(-camera.x, -camera.y, -camera.z));
}

Matrix4 Matrix4::Translate(const Vector3& translation) {
	Matrix4 result(1.0f);

	result.elements[3 + 0 * 4] = translation.x;
	result.elements[3 + 1 * 4] = translation.y;
	result.elements[3 + 2 * 4] = translation.z;

	return result;
}

Matrix4 Matrix4::Rotate(float angle, const Vector3& axis) {
	Matrix4 result(1.0f);

	float c = Math::Cos(angle);
	float s = Math::Sin(angle);
	float omc = 1.0f - c;

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	result.elements[0 + 0 * 4] = x * x * omc + c;
	result.elements[0 + 1 * 4] = y * x * omc + z * s;
	result.elements[0 + 2 * 4] = x * z * omc - y * s;

	result.elements[1 + 0 * 4] = x * y * omc - z * s;
	result.elements[1 + 1 * 4] = y * y * omc + c;
	result.elements[1 + 2 * 4] = y * z * omc + x * s;

	result.elements[2 + 0 * 4] = x * z * omc + y * s;
	result.elements[2 + 1 * 4] = y * z * omc - x * s;
	result.elements[2 + 2 * 4] = z * z * omc + c;

	return result;
}

Matrix4 Matrix4::Rotate(float x, float y, float z, float w) {
	Matrix4 result = Identity();

	float qx, qy, qz, qw, qx2, qy2, qz2, qxqx2, qyqy2, qzqz2, qxqy2, qyqz2, qzqw2, qxqz2, qyqw2, qxqw2;
	qx = x;
	qy = y;
	qz = z;
	qw = w;
	qx2 = (qx + qx);
	qy2 = (qy + qy);
	qz2 = (qz + qz);
	qxqx2 = (qx * qx2);
	qxqy2 = (qx * qy2);
	qxqz2 = (qx * qz2);
	qxqw2 = (qw * qx2);
	qyqy2 = (qy * qy2);
	qyqz2 = (qy * qz2);
	qyqw2 = (qw * qy2);
	qzqz2 = (qz * qz2);
	qzqw2 = (qw * qz2);

	result.rows[0] = Vector4(((1.0f - qyqy2) - qzqz2), (qxqy2 - qzqw2), (qxqz2 + qyqw2), 0.0f);
	result.rows[1] = Vector4((qxqy2 + qzqw2), ((1.0f - qxqx2) - qzqz2), (qyqz2 - qxqw2), 0.0f);
	result.rows[2] = Vector4((qxqz2 - qyqw2), (qyqz2 + qxqw2), ((1.0f - qxqx2) - qyqy2), 0.0f);
	return result;
}

Matrix4 Matrix4::Scale(const Vector3& scale) {
	Matrix4 result(1.0f);

	result.elements[0 + 0 * 4] = scale.x;
	result.elements[1 + 1 * 4] = scale.y;
	result.elements[2 + 2 * 4] = scale.z;

	return result;
}

Matrix4 Matrix4::Invert(const Matrix4& matrix) {
	Matrix4 result = matrix;
	return result.Invert();
}

Matrix4 Matrix4::Transpose(const Matrix4& matrix) {
	return Matrix4(
		Vector4(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x, matrix.rows[3].x),
		Vector4(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y, matrix.rows[3].y),
		Vector4(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z, matrix.rows[3].z),
		Vector4(matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w, matrix.rows[3].w)
	);
}

Matrix4 Matrix4::Lerp(const Matrix4& from, const Matrix4& to, float amount) {
	Matrix4 ret = Matrix4::Identity();
	for (int i = 0; i < 16; i++)
		ret.elements[i] = Math::Lerp(from.elements[i], to.elements[i], amount);
	return ret;
}

Matrix4 Matrix4::TRS(const Vector3& position, const Vector3& rotation, const Vector3& scale) {
	Matrix4 transform = Matrix4::Identity();
	transform *= Matrix4::Translate(position);
	transform *= Matrix4::Rotate(rotation.x, Vector3::XAxis());
	transform *= Matrix4::Rotate(rotation.y, Vector3::YAxis());
	transform *= Matrix4::Rotate(rotation.z, Vector3::ZAxis());
	transform *= Matrix4::Scale(scale);
	return transform;
}

