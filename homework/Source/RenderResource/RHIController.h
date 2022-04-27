/*************************************************

Author:乔南

Date:2022-4.01

Description:渲染资源控制器类,统一管理渲染资源的初始化和释放

**************************************************/
#pragma once
#include "./Material/Material.h"
#include "./StaticMesh/StaticMesh.h"
#include "./Texture/StaticTexture.h"
#include "../scene/Light.h"
#include "../scene/SkyBox.h"


class RHIController
{
public:
	void AddResource(StaticMeshRef ref) { resources_staticmesh.push_back(ref); }
	void AddResource(StaticTexture2DRef ref) { resources_statictexture2D.push_back(ref); }
	void AddResource(StaticTextureCubeMapRef ref) { resources_statictextureCubeMap.push_back(ref); }
	void AddResource(LightRef ref) { resources_light.push_back(ref); }
	void AddResource(SkyBoxRef ref) { resources_sky.push_back(ref); }

	void InitRHI()
	{
		for (int i = 0; i < resources_staticmesh.size(); i++) { resources_staticmesh[i]->InitRHI(); }
		for (int i = 0; i < resources_statictexture2D.size(); i++) { resources_statictexture2D[i]->InitRHI(); }
		for (int i = 0; i < resources_statictextureCubeMap.size(); i++) { resources_statictextureCubeMap[i]->InitRHI(); }
		for (int i = 0; i < resources_light.size(); i++) { resources_light[i]->InitRHI(); }
		for (int i = 0; i < resources_sky.size(); i++) { resources_sky[i]->InitRHI(); }
	}

	void ReleaseRHI()
	{
		for (int i = 0; i < resources_staticmesh.size(); i++) { resources_staticmesh[i]->ReleaseRHI(); }
		for (int i = 0; i < resources_statictexture2D.size(); i++) { resources_statictexture2D[i]->ReleaseRHI(); }
		for (int i = 0; i < resources_statictextureCubeMap.size(); i++) { resources_statictextureCubeMap[i]->ReleaseRHI(); }
		for (int i = 0; i < resources_light.size(); i++) { resources_light[i]->ReleaseRHI(); }
		for (int i = 0; i < resources_sky.size(); i++) { resources_sky[i]->ReleaseRHI(); }
	}

private:
	std::vector <StaticMeshRef> resources_staticmesh;
	std::vector <StaticTexture2DRef> resources_statictexture2D;
	std::vector <StaticTextureCubeMapRef> resources_statictextureCubeMap;
	std::vector <LightRef> resources_light;
	std::vector <SkyBoxRef> resources_sky;
};
