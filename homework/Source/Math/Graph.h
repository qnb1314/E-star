/*************************************************

Author:����

Date:2022-3.28

Description:������,ͨ�����ֹ��캯������������Ԫ����

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

//��
class GraphBall :public Graph
{
public:
	//num_x��ʾ����������num_y��ʾγ������
	GraphBall(int num_x, int num_y);
};


//������
class GraphRectangle :public Graph
{
public:
	//��׼��+���������ȵķ�������
	GraphRectangle(Vector3f point, Vector3f vectorx, Vector3f vectory);
};


//������
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