/*************************************************

Author:乔南

Date:2022-3.26

Description:包围体类，采用AABB包围盒求交，加速场景光线求交

**************************************************/
#pragma once
#include "../Math/Intersection.h"
#include "../Math/Ray.h"

class Bound
{
public:
	Vector3f PMax, PMin;

	Bound(Vector3f pmax, Vector3f pmin) : PMax(pmax), PMin(pmin) {}

	void Update(Vector3f pmax, Vector3f pmin) { PMax = pmax; PMin = pmin; }

	void GetPMaxMin(Vector3f& pMax, Vector3f& pMin) { pMax = PMax; pMin = PMin; }

	Intersection GetIntersectionWithRay(const Ray& ray)
	{
		Intersection inter;

		Vector3f tpmin, tpmax;
		tpmin = (PMin - ray.origin) * ray.direction_inv;
		tpmax = (PMax - ray.origin) * ray.direction_inv;
		Vector3f tmin, tmax;
		tmin = Min(tpmin, tpmax), tmax = Max(tpmin, tpmax);
		float t_in = Max(tmin);
		float t_out = Min(tmax);

		if (t_in > t_out)return inter;
		if (t_out < 0)return inter;

		inter.happened = true;
		if (t_in <= 0)inter.distance = t_out;
		else inter.distance = t_in;

		return inter;
	}
};