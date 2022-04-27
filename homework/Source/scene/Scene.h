/*************************************************

Author:����

Date:2022-3.29

Description:������,������Դ�������Լ���պ�

**************************************************/
#pragma once
#include <vector>
#include "Object.h"
#include "Light.h"
#include "SkyBox.h"

class Scene
{
public:
	std::vector <ObjectRef> All_Object;
	std::vector <LightRef> All_Light;

	std::vector <SkyBoxRef> Environment;
	unsigned int sky_state = 0;

	Scene() {}
	~Scene()
	{
	}

	Intersection GetIntersectionWithOutBVH(const Ray& ray)
	{
		Intersection inter;
		for (int i = 0; i < All_Object.size(); i++)
		{
			Intersection temp = All_Object[i]->GetIntersection(ray);
			if (temp.happened && temp.distance < inter.distance)inter = temp;
		}
		return inter;
	}

	void AddObject(Object* object) { All_Object.push_back(ObjectRef(object)); }

	void AddLight(LightRef Lightref)
	{
		//Ҫ�жϹ�Դ�Ƿ��ظ�����
		for (int i = 0; i < All_Light.size(); i++)
		{
			if (Lightref == All_Light[i])
			{
				std::cout << "ERROR in Scene::AddLight:�ظ���ӹ�Դ!" << std::endl;
				return;
			}
		}
		All_Light.push_back(Lightref);
	}

	void AddSkyBox(SkyBoxRef SkyBoxref)
	{
		Environment.push_back(SkyBoxref);
	}

	void ChangeSkyState()
	{
		sky_state++;
		if (sky_state >= Environment.size())sky_state = 0;
	}
};

