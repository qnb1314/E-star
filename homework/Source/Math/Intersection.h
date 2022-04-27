/*************************************************

Author:乔南

Date:2022-3.24

Description:交点信息类

**************************************************/
#pragma once
#include <Eigen/Dense>
class Object;
class Intersection
{
public:
	bool happened;
	float distance;
	Object* object;
	Intersection():happened(false), distance(std::numeric_limits<float>::max()), object(NULL){}
	~Intersection(){}
};

