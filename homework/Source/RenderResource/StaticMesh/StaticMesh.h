/*************************************************

Author:����

Date:2022-4.01

Description:staticmesh��,����cpu��������Դ,���Դ������ͷ�GPU�˶���������

**************************************************/
#pragma once
#include "../RenderResource.h"
#include "StaticMeshRenderData.h"

class StaticMesh:public RenderResource
{
public:
	Graph* graph;
	StaticMeshRenderData renderdata;
	StaticMesh(Graph* g) :graph(g), renderdata() {}
	~StaticMesh() { delete graph; }

	Intersection GetIntersection(const Ray& ray, Eigen::Matrix4f Model) const { return graph->GetIntersectionWithRay(ray, Model); }
	void GetPMaxPMin(Vector3f& pMax, Vector3f& pMin, Eigen::Matrix4f Model) const { graph->GetPMaxPMin(pMax, pMin, Model); }

	virtual void InitRHI() override { renderdata.InitRHI(graph); }
	virtual void ReleaseRHI() override { renderdata.ReleaseRHI(); }
};
typedef CountRef<StaticMesh> StaticMeshRef;