/*************************************************

Author:乔南

Date:2022-4.01

Description:三角面元类,拥有光线求交、获取包围体以及切向量的功能

**************************************************/
#pragma once
#include "Ray.h"
#include "Intersection.h"
#include "MathFunction.h"
class Triangle
{
public:
	Vector3f A, B, C;//顶点
	Vector3f An, Bn, Cn;//顶点法线
	Vector3f At, Bt, Ct;//顶点纹理坐标

	Triangle(Vector3f a, Vector3f b, Vector3f c, Vector3f an, Vector3f bn, Vector3f cn, Vector3f at, Vector3f bt, Vector3f ct)
		:A(a), B(b), C(c), An(an), Bn(bn), Cn(cn), At(at), Bt(bt), Ct(ct){}

	Intersection GetIntersectionWithRay(const Ray& ray, Eigen::Matrix4f Model) const
	{
		//变换到世界坐标
		Vector3f A_world = Model * A;
		Vector3f B_world = Model * B;
		Vector3f C_world = Model * C;
		Vector3f E1 = B_world - A_world;
		Vector3f E2 = C_world - A_world;
		Intersection inter;
		//1.计算中间量S，S1，S1E1，S1E1_inv（S2可放在之后计算）
		Vector3f S = ray.origin - A_world, S1 = CrossProduction(ray.direction, E2);
		double S1E1 = DotProduction(S1, E1);
		if (fabs(S1E1) < DEVIA)return inter;
		double S1E1_inv = 1.0f / S1E1;

		//2.按照先后计算b1,t,b2
		float b1 = (float)(DotProduction(S1, S) * S1E1_inv);
		if (b1 > 1.0f || b1 < .0f)return inter;
		Vector3f S2 = CrossProduction(S, E1);
		float t = (float)(DotProduction(S2, E2) * S1E1_inv);
		if (t < 0)return inter;
		float b2 = (float)(DotProduction(S2, ray.direction) * S1E1_inv);
		if (b2 > 1.0f || b2 < .0f || b1 + b2>1.0f)return inter;

		//3.相交，计算交点信息
		inter.distance = t;
		inter.happened = true;
		return inter;
	}

	void GetPMaxPMin(Vector3f& PMax, Vector3f& PMin, Eigen::Matrix4f Model) const
	{
		//变换到世界坐标
		Vector3f A_world = Model * A;
		Vector3f B_world = Model * B;
		Vector3f C_world = Model * C;
		PMax = Max(A_world, Max(B_world, C_world));
		PMin = Min(A_world, Min(B_world, C_world));
	}

	void GetTB(Vector3f& T_vec, Vector3f& B_vec) const
	{
		Eigen::Matrix<float, 3, 2> mat1;
		mat1 << 
			C.x - A.x, B.x - A.x,
			C.y - A.y, B.y - A.y,
			C.z - A.z, B.z - A.z;
		Eigen::Matrix<float, 2, 2> mat2;
		mat2 <<
			Ct.x - At.x, Bt.x - At.x,
			Ct.y - At.y, Bt.y - At.y;
		Eigen::Matrix<float, 3, 2> answer;
		answer = mat1 * (mat2.inverse().eval());
		T_vec = Vector3f(answer(0, 0), answer(1, 0), answer(2, 0)).normalized();
		B_vec = Vector3f(answer(0, 1), answer(1, 1), answer(2, 1)).normalized();
	}
};

