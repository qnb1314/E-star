/*************************************************

Author:����

Date:2022-3.26

Description:��ѧ������

**************************************************/
#pragma once
#include <vector>
#include "Vector.h"

#undef M_PI
#define M_PI 3.1415926f
#undef INV_PI
#define INV_PI 0.3183098f

//��λ������ת��λ����-ͷ����������߷���theta��ʾ���ǣ�phy��ʾ��y����ϵ�Ƕȣ�
void GetViewAndUpViewWithAngle(const float&, const float&, Vector3f&, Vector3f&);

void GetAngleWithVector(const Vector3f&, float&, float&);//��λ����תΪ������
void GetVectorWithAngle(const float&, const float&, Vector3f&);//������תΪ��λ����
float GetHalfAngleTan(const float&);//�Ƕ��ư������ֵ

inline float clamp(const float& i, const float& j, const float& x){return std::max(i, std::min(j, x));}

//��ת������
Eigen::Matrix4f RotateOn_X_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Y_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Z_WithRadian(float theta);
Eigen::Matrix4f RotateOn_Any_WithRadian(float theta, Vector3f A);

//ƽ�ƾ�����
Eigen::Matrix4f MoveWithVector(Vector3f move);

//���ž�����
Eigen::Matrix4f ZoomWithVector(Vector3f zoom);

//ʩ����������
void Gram_Schmidt(const Vector3f& N, Vector3f& T, Vector3f& B);

//Eigen
void GetMat4WithMatrix4f(Eigen::Matrix4f Mat, float mat[16]);

//����ƽ��λ�ù�ϵ
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