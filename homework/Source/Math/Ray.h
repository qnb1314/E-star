/*************************************************

Author:乔南

Date:2022-3.25

Description:几何光线类,用于光线求交

**************************************************/
#pragma once
#include"Vector.h"
class Ray
{
public:
	Vector3f origin;
	Vector3f direction,direction_inv;
	Ray(Vector3f ori, Vector3f dir) :origin(ori), direction(dir), direction_inv(dir.inv()) {}
};

