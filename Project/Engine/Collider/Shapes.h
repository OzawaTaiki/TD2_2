#pragma once

struct Sphere
{
    float radius;
    float center[3];
};

struct AABB
{
    float min[3];
    float max[3];
    float referencePoint[3];
    float center[3];

    AABB() = default;
    AABB(const float* _min, const float* _max) : referencePoint{ 0.0f, 0.0f, 0.0f }
    {
        for (int i = 0; i < 3; ++i)
        {
            if (min[0] < max[0])
            {
                min[i] = _min[i];
                max[i] = _max[i];
            }
            else
            {
                min[i] = _max[i];
                max[i] = _min[i];
            }
        }

    };
};

struct OBB
{
    // 最小値
    float min[3];
    // 最大値
    float max[3];
    // 基準点
    float referencePoint[3];
    // 各軸の方向ベクトル
    float orientations[3][3];
    // 中心座標
    float center[3];

    OBB() = default;
    OBB(const float* _min, const float* _max) : referencePoint{ 0.0f, 0.0f, 0.0f },
        orientations{
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f} }
    {
        for (int i = 0; i < 3; ++i)
        {
            if (min[0] < max[0])
            {
                min[i] = _min[i];
                max[i] = _max[i];
            }
            else
            {
                min[i] = _max[i];
                max[i] = _min[i];
            }
        }
    };

    void CalculateOrientations();
};