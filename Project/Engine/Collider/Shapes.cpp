#include "Shapes.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"


void OBB::Calculate(const Matrix4x4& _worldMat)
{
    CalculateOrientations(_worldMat);
    CalculateMinMax(_worldMat);
    CalculateVertices(_worldMat);
}

void OBB::CalculateOrientations(const Matrix4x4& _worldMat)
{
    orientations[0].x = _worldMat.m[0][0];
    orientations[0].y = _worldMat.m[0][1];
    orientations[0].z = _worldMat.m[0][2];

    orientations[1].x = _worldMat.m[1][0];
    orientations[1].y = _worldMat.m[1][1];
    orientations[1].z = _worldMat.m[1][2];

    orientations[2].x = _worldMat.m[2][0];
    orientations[2].y = _worldMat.m[2][1];
    orientations[2].z = _worldMat.m[2][2];

    orientations[0] = orientations[0].Normalize();
    orientations[1] = orientations[1].Normalize();
    orientations[2] = orientations[2].Normalize();
}

void OBB::CalculateMinMax(const Matrix4x4& _worldMat)
{
    Vector3 scale = _worldMat.GetScale();
    min = (localMin) - referencePoint;
    max = (localMax) - referencePoint;

    for (int i = 0; i < 3; ++i)
    {
        if (min[i] > max[i])
        {
            std::swap(min[i], max[i]);
        }
    }
}

void OBB::CalculateVertices(const Matrix4x4& _worldMat)
{
    Vector3 rotMin[3];
    Vector3 rotMax[3];

    for (int i = 0; i < 3; ++i)
    {
        rotMin[i] = min * orientations[i];
        rotMax[i] = max * orientations[i];
    }

    vertices[0] = max;
    vertices[1] = Vector3(max.x, max.y, min.z);
    vertices[2] = Vector3(max.x, min.y, max.z);
    vertices[3] = Vector3(max.x, min.y, min.z);
    vertices[4] = Vector3(min.x, max.y, max.z);
    vertices[5] = Vector3(min.x, max.y, min.z);
    vertices[6] = Vector3(min.x, min.y, max.z);
    vertices[7] = min;

    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = Transform(vertices[i], _worldMat);
    }

    min = vertices[0];
    max = vertices[0];

    for (int i = 1; i < 8; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (min[j] > vertices[i][j])
                min[j] = vertices[i][j];
            if (max[j] < vertices[i][j])
                max[j] = vertices[i][j];
        }
    }
}

void AABB::CalculateMinMax()
{
    for (int i = 0; i < 3; ++i)
    {
        if (min[i] < max[i])
        {
            min[i] = referencePoint[i] + min[i];
            max[i] = referencePoint[i] + max[i];
        }
        else
        {
            min[i] = referencePoint[i] + max[i];
            max[i] = referencePoint[i] + min[i];
        }
    }
}
