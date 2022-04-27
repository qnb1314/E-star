/*************************************************

Author:乔南

Date:2022-3.26

Description:数学函数库

**************************************************/
#pragma once
#include <vector>
#include "Vector.h"

#undef M_PI
#define M_PI 3.1415926f
#undef INV_PI
#define INV_PI 0.3183098f

//单位球坐标转单位向量-头顶方向和视线方向（theta表示仰角，phy表示绕y右手系角度）
void GetViewAndUpViewWithAngle(const float&, const float&, Vector3f&, Vector3f&);

void GetAngleWithVector(const Vector3f&, float&, float&);//单位向量转为球坐标
void GetVectorWithAngle(const float&, const float&, Vector3f&);//球坐标转为单位向量
float GetHalfAngleTan(const float&);//角度制半角正切值

inline float clamp(const float& i, const float& j, const float& x){return std::max(i, std::min(j, x));}

//旋转矩阵构造
Eigen::Matrix4f RotateOn_X_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Y_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Z_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Any_WithRadian(float theta, Vector3f A);

//平移矩阵构造
Eigen::Matrix4f MoveWithVector(Vector3f move);

//缩放矩阵构造
Eigen::Matrix4f ZoomWithVector(Vector3f zoom);

//施密特正交法
void Gram_Schmidt(const Vector3f& N, Vector3f& T, Vector3f& B);

//Eigen
void GetMat4WithMatrix4f(Eigen::Matrix4f Mat, float mat[16]);

//点与平面位置关系
inline bool PointWithPlane(Vector3f Point, Vector3f Normal, Vector3f CheckPoint) { return DotProduction(Point - CheckPoint, Normal) < 0; }
inline void GetNearFarOfAABB(Vector3f Normal, Vector3f cube[2][2][2], Vector3f& Near, Vector3f& Far)
{
	bool x = Normal.x < 0, y = Normal.y < 0, z = Normal.z < 0;
	Near = cube[x][y][z];
	Far = cube[!x][!y][!z];
}
inline int AABBWithView(const std::vector <Vector3f>& normals, const std::vector <Vector3f>& points, Vector3f PMax, Vector3f PMin)
{
	Vector3f cube[2][2][2];
	for (int i = 0; i <= 1; i++)
	{
		for (int j = 0; j <= 1; j++)
		{
			for (int k = 0; k <= 1; k++)
			{
				cube[i][j][k].x = ((i == 0) ? PMin.x : PMax.x);
				cube[i][j][k].y = ((j == 0) ? PMin.y : PMax.y);
				cube[i][j][k].z = ((k == 0) ? PMin.z : PMax.z);
			}
		}
	}
	bool in = true;
	for (int i = 0; i < 6; i++)
	{
		Vector3f near, far;
		GetNearFarOfAABB(normals[i], cube, near, far);
		if (PointWithPlane(points[i], normals[i], near))return 1;
		in = in && (!PointWithPlane(points[i], normals[i], far));
	}
	if (in)return -1;
	else return 0;
}