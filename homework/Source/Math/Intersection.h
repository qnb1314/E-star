/*************************************************

Author:����

Date:2022-3.24

Description:������Ϣ��

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

