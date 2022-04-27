/*************************************************

Author:乔南

Date:2022-3.28

Description:几何类,通过各种构造函数生成三角面元网格

**************************************************/
#pragma once
#include <vector>
#include "Triangle.h"
#include "Ray.h"
class Graph
{
public:
	std::vector <Triangle> Triangles;
	Graph(){}
	virtual Intersection GetIntersectionWithRay (const Ray& ray, Eigen::Matrix4f Model) const
	{
		Intersection inter;
		for (int i = 0; i < Triangles.size(); i++)
		{
			Intersection temp = Triangles[i].GetIntersectionWithRay(ray, Model);
			if (temp.happened && temp.distance < inter.distance)inter = temp;
		}
		return inter;
	}
	virtual void GetPMaxPMin(Vector3f& pMax, Vector3f& pMin, Eigen::Matrix4f Model) const
	{
		Vector3f pmax(-1e6), pmin(1e6);
		for (int i = 0; i < Triangles.size(); i++)
		{
			Vector3f temp_max, temp_min;
			Triangles[i].GetPMaxPMin(temp_max, temp_min, Model);
			pmax = Max(pmax, temp_max);
			pmin = Min(pmin, temp_min);
		}
		pMax = pmax, pMin = pmin;
	}
};

//球
class GraphBall :public Graph
{
public:
	//num_x表示经线条数，num_y表示纬线条数
	GraphBall(int num_x, int num_y);
};


//长方形
class GraphRectangle :public Graph
{
public:
	//基准点+两个带长度的方向向量
	GraphRectangle(Vector3f point, Vector3f vectorx, Vector3f vectory);
};


//长方体
class GraphCuboid :public Graph
{
public:
	GraphCuboid(float lx, float ly, float lz);
};


//obj
class GraphObj :public Graph
{
public:
	GraphObj(std::string objpath);
};