/*************************************************

Author:����

Date:2022-3.21

Description:��ά������

**************************************************/
#pragma once
#include <iostream>
#include <cmath>
#include <Eigen/Dense>
#undef DEVIA//���ƫ����
#define DEVIA 0.001f

class Vector3f
{
public:
	float x, y, z;

	//���캯��
	Vector3f(float xx) :x(xx), y(xx), z(xx) {}
	Vector3f(float xx, float yy, float zz) :x(xx), y(yy), z(zz) {}
	Vector3f() :x(.0f), y(.0f), z(.0f) {}

	//����ֵ����
	float norm() { return sqrtf(x * x + y * y + z * z); }
	Vector3f normalized()
	{
		float norm = this->norm();
		return Vector3f(x / norm, y / norm, z / norm);
	}
	//���ص�������
	Vector3f inv()
	{
		float xx = x, yy = y, zz = z;

		if (xx < DEVIA && xx >= 0)xx = DEVIA;
		else if (xx > -DEVIA && xx < 0)xx = -DEVIA;

		if (yy < DEVIA && yy >= 0)yy = DEVIA;
		else if (yy > -DEVIA && yy < 0)yy = -DEVIA;

		if (zz < DEVIA && zz >= 0)zz = DEVIA;
		else if (zz > -DEVIA && zz < 0)zz = -DEVIA;

		return Vector3f(1.0f / xx, 1.0f / yy, 1.0f / zz);
	}
	//��������غ���
	//����
	friend Vector3f operator*(const Vector3f& A, const float& a) { return Vector3f(A.x * a, A.y * a, A.z * a); }
	friend Vector3f operator*(const float& a, const Vector3f& A) { return Vector3f(A.x * a, A.y * a, A.z * a); }
	friend Vector3f operator/(const Vector3f& A, const float& a) { return Vector3f(A.x / a, A.y / a, A.z / a); }
	friend Vector3f operator-(const Vector3f& A) { return Vector3f(-1 * A.x, -1 * A.y, -1 * A.z); }
	//������Ӧֵ����
	friend Vector3f operator+(const Vector3f& A, const Vector3f& B) { return Vector3f(A.x + B.x, A.y + B.y, A.z + B.z); }
	friend Vector3f operator-(const Vector3f& A, const Vector3f& B) { return Vector3f(A.x - B.x, A.y - B.y, A.z - B.z); }
	friend Vector3f operator*(const Vector3f& A, const Vector3f& B) { return Vector3f(A.x * B.x, A.y * B.y, A.z * B.z); }
	//���Ժ���
	friend std::ostream& operator<<(std::ostream& os, const Vector3f& v) { return os << "(" << v.x << "," << v.y << "," << v.z << ")"; }

	//�����Ժ���
	friend float DotProduction(const Vector3f& A, const Vector3f& B) { return A.x * B.x + A.y * B.y + A.z * B.z; }
	friend Vector3f CrossProduction(const Vector3f& A, const Vector3f& B) { return Vector3f(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - B.x * A.y); }
	friend Vector3f Max(const Vector3f& A, const Vector3f& B) { return Vector3f(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
	friend Vector3f Min(const Vector3f& A, const Vector3f& B) { return Vector3f(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }

	friend float Max(const Vector3f& v) { return std::max(v.x, std::max(v.y, v.z)); }
	friend float Min(const Vector3f& v) { return std::min(v.x, std::min(v.y, v.z)); }

	//��������������
	friend Vector3f operator*(const Eigen::Matrix4f& M, const Vector3f& v)
	{
		Eigen::Vector4f res_eigen = M * Eigen::Vector4f(v.x, v.y, v.z, 1);
		return Vector3f(res_eigen.x(), res_eigen.y(), res_eigen.z()) * (1.0f / res_eigen.w());
	}
};

