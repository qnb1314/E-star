/*************************************************

Author:乔南

Date:2022-4.01

Description:观察类,用于视角变换以及构造VP矩阵

**************************************************/
#pragma once
#include "../Math/MathFunction.h"

class View
{
public:
	Vector3f ViewPoint;
	float Theta, Phy;//右手方位角和仰角
    Eigen::Matrix4f ProjectionMatrix;

    unsigned int width, height;
    float alpha;
    float near, far;

    View()
        :ViewPoint(Vector3f(0,0,0)), Theta(0), Phy(0), width(1), height(1), near(0.1f), far(50.0f), alpha(90)
    {}

    View(Vector3f viewpoint, float theta, float phy, unsigned int w, unsigned int h, float n, float f, float a)
        :ViewPoint(viewpoint), Theta(theta), Phy(phy), width(w), height(h), near(n), far(f), alpha(a)
    {
        SetProjectionMatrix();
    }

    void Update()
    {
        SetProjectionMatrix();
    }

    Eigen::Matrix4f GetViewMatrix() const
    {
        Vector3f ViewVector, UpView;
        GetViewAndUpViewWithAngle(Theta, Phy, ViewVector, UpView);
        Vector3f Right = CrossProduction(ViewVector, UpView);
        Eigen::Matrix4f ViewMatrix;
        ViewMatrix << Right.x, UpView.x, -ViewVector.x, ViewPoint.x,
            Right.y, UpView.y, -ViewVector.y, ViewPoint.y,
            Right.z, UpView.z, -ViewVector.z, ViewPoint.z,
            0.0f, 0.0f, 0.0f, 1.0f;
        ViewMatrix = ViewMatrix.inverse().eval();
        return ViewMatrix;
    }
    Eigen::Matrix4f GetProjectionMatrix() const
    {
        return ProjectionMatrix;
    }
    Eigen::Matrix4f GetVPMatrix() const
    {
        return ProjectionMatrix * GetViewMatrix();
    }

    void GetViewPlane(std::vector <Vector3f>& normals, std::vector <Vector3f>& points) const
    {
        Vector3f ViewVector, UpView;
        GetViewAndUpViewWithAngle(Theta, Phy, ViewVector, UpView);
        Vector3f Right = CrossProduction(ViewVector, UpView);

        Vector3f near_center_vec = near * ViewVector;
        Vector3f near_center = ViewPoint + near_center_vec;
        Vector3f far_center = ViewPoint + far * ViewVector;

        float aspect = (float)width / (float)height;
        float b_half = near * tan(alpha * M_PI / 360.0f);
        float a_half = b_half * aspect;

        Vector3f near_left_vec = near_center_vec - a_half * Right;
        Vector3f near_right_vec = near_center_vec + a_half * Right;
        Vector3f near_up_vec = near_center_vec + b_half * UpView;
        Vector3f near_down_vec = near_center_vec - b_half * UpView;

        //左
        normals.push_back(CrossProduction(UpView, near_left_vec).normalized());
        points.push_back(ViewPoint);
        //右
        normals.push_back(CrossProduction(near_right_vec, UpView).normalized());
        points.push_back(ViewPoint);
        //上
        normals.push_back(CrossProduction(Right, near_up_vec).normalized());
        points.push_back(ViewPoint);
        //下
        normals.push_back(CrossProduction(near_down_vec, Right).normalized());
        points.push_back(ViewPoint);
        //前
        normals.push_back(-ViewVector);
        points.push_back(near_center);
        //后
        normals.push_back(ViewVector);
        points.push_back(far_center);
    }

public:
    //相机变换函数
    void TurnUp(float delta)
    {
        Phy += delta;
    }
    void TurnLeft(float delta)
    {
        Theta += delta;
    }
    void GoForward(float step)
    {
        Vector3f ViewVector, UpView;
        GetViewAndUpViewWithAngle(Theta, Phy, ViewVector, UpView);
        ViewPoint = ViewPoint + ViewVector * step;
    }
    void GoRight(float step)
    {
        Vector3f ViewVector, UpView;
        GetViewAndUpViewWithAngle(Theta, Phy, ViewVector, UpView);
        Vector3f Right = CrossProduction(ViewVector, UpView);
        ViewPoint = ViewPoint + Right * step;
    }

private:
	void SetProjectionMatrix()
	{
        float aspect = (float)width / (float)height;
        //该矩阵映射完会按照XOY平面对称,使得深度值越小越近。
        Eigen::Matrix4f projection;
        projection <<
            near, 0, 0, 0,
            0, near, 0, 0,
            0, 0, near + far, far* near,
            0, 0, -1, 0;

        Eigen::Matrix4f translation;
        translation <<
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0.5f * near + 0.5f * far,
            0, 0, 0, 1;

        float c = far - near;
        float b = 2 * near * tan(alpha * M_PI / 360.0f);
        float a = b * aspect;
        Eigen::Matrix4f zoom;
        zoom <<
            2.0f / a, 0, 0, 0,
            0, 2.0f / b, 0, 0,
            0, 0, -2.0f / c, 0,
            0, 0, 0, 1;

        ProjectionMatrix = zoom * translation * projection;
	}
};

