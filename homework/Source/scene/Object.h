/*************************************************

Author:乔南

Date:2022-3.27

Description:场景对象类,包含StaticMesh、材质、包围体以及模-世变换矩阵

**************************************************/
#pragma once
#include "../RenderResource/StaticMesh/StaticMesh.h"
#include "../RenderResource/Material/Material.h"
#include "Bound.h"
class Object
{
public:
	StaticMeshRef staticmeshref;
	MaterialRef materialref;
	Bound* bound;
	Eigen::Matrix4f Model;

	Object(StaticMeshRef mesh, MaterialRef material) :staticmeshref(mesh), materialref(material)
	{
		Model <<
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		Vector3f pmax, pmin;
		GetPMaxPMin(pmax, pmin);
		bound = new Bound(pmax, pmin);
	}
	~Object() { delete bound; }

	Intersection GetIntersection(const Ray& ray)
	{
		Intersection inter;
		inter = bound->GetIntersectionWithRay(ray);
		if (!inter.happened)return inter;

		inter = staticmeshref->GetIntersection(ray, Model);
		if (!inter.happened)return inter;

		inter.object = this;
		return inter;
	}

	void GetPMaxPMin(Vector3f& pMax, Vector3f& pMin) { staticmeshref->GetPMaxPMin(pMax, pMin, Model); }

	void UpdateBound()
	{
		Vector3f pmax, pmin;
		GetPMaxPMin(pmax, pmin);
		bound->Update(pmax, pmin);
	}

	//仿射变换函数
	void RotateX(float theta)
	{
		float theta_radian = theta / 180.0f * M_PI;
		Vector3f move(Model(0, 3), Model(1, 3), Model(2, 3));

		Eigen::Matrix4f Move_inv = MoveWithVector(-move);
		Eigen::Matrix4f Rotate = RotateOn_X_WithRadian(theta_radian);
		Eigen::Matrix4f Move = MoveWithVector(move);
		Eigen::Matrix4f ChangeMat = Move * Rotate * Move_inv;

		Model = ChangeMat * Model;
		UpdateBound();
	}
	void RotateY(float theta)
	{
		float theta_radian = theta / 180.0f * M_PI;
		Vector3f move(Model(0, 3), Model(1, 3), Model(2, 3));

		Eigen::Matrix4f Move_inv = MoveWithVector(-move);
		Eigen::Matrix4f Rotate = RotateOn_Y_WithRadian(theta_radian);
		Eigen::Matrix4f Move = MoveWithVector(move);
		Eigen::Matrix4f ChangeMat = Move * Rotate * Move_inv;

		Model = ChangeMat * Model;
		UpdateBound();
	}
	void RotateZ(float theta)
	{
		float theta_radian = theta / 180.0f * M_PI;
		Vector3f move(Model(0, 3), Model(1, 3), Model(2, 3));

		Eigen::Matrix4f Move_inv = MoveWithVector(-move);
		Eigen::Matrix4f Rotate = RotateOn_Z_WithRadian(theta_radian);
		Eigen::Matrix4f Move = MoveWithVector(move);
		Eigen::Matrix4f ChangeMat = Move * Rotate * Move_inv;

		Model = ChangeMat * Model;
		UpdateBound();
	}

	void MoveBy(Vector3f step)
	{
		Eigen::Matrix4f ChangeMat = MoveWithVector(step);
		Model = ChangeMat * Model;
		UpdateBound();
	}

	void MoveTo(Vector3f target)
	{
		Vector3f now_position(Model(0, 3), Model(1, 3), Model(2, 3));
		Vector3f step = target - now_position;

		Eigen::Matrix4f ChangeMat = MoveWithVector(step);
		Model = ChangeMat * Model;
		UpdateBound();
	}

	void Zoom(Vector3f ratio)
	{
		Vector3f move(Model(0, 3), Model(1, 3), Model(2, 3));

		Eigen::Matrix4f Move_inv = MoveWithVector(-move);
		Eigen::Matrix4f Zoom = ZoomWithVector(ratio);
		Eigen::Matrix4f Move = MoveWithVector(move);

		Eigen::Matrix4f ChangeMat = Move * Zoom * Move_inv;

		Model = ChangeMat * Model;
		UpdateBound();
	}
};
typedef CountRef<Object> ObjectRef;