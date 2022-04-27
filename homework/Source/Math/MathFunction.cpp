#include "MathFunction.h"
#include <cmath>

Eigen::Matrix4f RotateOn_X_WithRadian(float theta)
{
    Eigen::Matrix4f res;
    float cos_theta = cosf(theta), sin_theta = sinf(theta);
    res <<
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos_theta, -sin_theta, 0.0f,
        0.0f, sin_theta, cos_theta, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

Eigen::Matrix4f RotateOn_Y_WithRadian(float theta)
{
    Eigen::Matrix4f res;
    float cos_theta = cosf(theta), sin_theta = sinf(theta);
    res <<
        cos_theta, 0.0f, sin_theta, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -sin_theta, 0.0f, cos_theta, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

Eigen::Matrix4f RotateOn_Z_WithRadian(float theta)
{
    Eigen::Matrix4f res;
    float cos_theta = cosf(theta), sin_theta = sinf(theta);
    res <<
        cos_theta, -sin_theta, 0.0f, 0.0f,
        sin_theta, cos_theta, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

Eigen::Matrix4f RotateOn_Any_WithRadian(float theta, Vector3f A)
{
    Eigen::Matrix4f res;
    float c = cos(theta);
    float s = sin(theta);
    float Ax = A.x, Ay = A.y, Az = A.z;
    res << c + (1 - c) * Ax * Ax, (1 - c)* Ax* Ay - s * Az, (1 - c)* Ax* Az + s * Ay, 0.0f,
        (1 - c)* Ax* Ay + s * Az, c + (1 - c) * Ay * Ay, (1 - c)* Ay* Az - s * Ax, 0.0f,
        (1 - c)* Ax* Az - s * Ay, (1 - c)* Ay* Az + s * Ax, c + (1 - c) * Az * Az, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

Eigen::Matrix4f MoveWithVector(Vector3f move)
{
    Eigen::Matrix4f res;
    res <<
        1.0f, 0.0f, 0.0f, move.x,
        0.0f, 1.0f, 0.0f, move.y,
        0.0f, 0.0f, 1.0f, move.z,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

Eigen::Matrix4f ZoomWithVector(Vector3f zoom)
{
    Eigen::Matrix4f res;
    res <<
        zoom.x, 0.0f, 0.0f, 0.0f,
        0.0f, zoom.y, 0.0f, 0.0f,
        0.0f, 0.0f, zoom.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f;
    return res;
}

void Gram_Schmidt(const Vector3f& N, Vector3f& T, Vector3f& B)
{
    T = T - DotProduction(N, T) * N;
    B = B - DotProduction(N, B) * N - DotProduction(T, B) * T;
    T = T.normalized();
    B = B.normalized();
}

void GetViewAndUpViewWithAngle(const float& theta, const float& phy, Vector3f& viewvector, Vector3f& upview)
{
    //先将viewvector和upview定位x、y轴
    viewvector = Vector3f(1.0f, 0.0f, 0.0f);
    upview = Vector3f(0.0f, 1.0f, 0.0f);
    
    //再把角度转为弧度制
    float theta_radian = M_PI * theta / 180.0f;
    float phy_radian = M_PI * phy / 180.0f;

    Eigen::Matrix4f rotate_Z_Y = RotateOn_Y_WithRadian(theta_radian) * RotateOn_Z_WithRadian(phy_radian);

    viewvector = rotate_Z_Y * viewvector;
    upview = rotate_Z_Y * upview;
}

void GetAngleWithVector(const Vector3f& v, float& theta, float& phy)
{
    theta = acosf(v.y);
    phy = atanf(v.z / v.x);
    if (v.x < 0)phy = phy + M_PI;
    else if (v.z < 0)phy = phy + 2 * M_PI;
}

void GetVectorWithAngle(const float& theta, const float& phy, Vector3f& vector)
{
    float k = M_PI / 180.0f;
    float theta_radian = k * theta;
    float phy_radian = k * phy;
    vector = Vector3f(sinf(theta_radian) * cosf(phy_radian), cosf(theta_radian), sinf(theta_radian) * sinf(phy_radian));
}

float GetHalfAngleTan(const float& theta)
{
    return tanf(theta * M_PI / 360.0f);
}

void GetMat4WithMatrix4f(Eigen::Matrix4f Mat, float mat[16])
{
    int num = 0;
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            mat[num++] = Mat(i, j);
        }
    }
}
