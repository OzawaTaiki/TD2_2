#pragma once

#include "Camera.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include <d3d12.h>
#include <wrl.h>
#include <array>
class LineDrawer
{
public:

    static LineDrawer* GetInstance();

    void Initialize();
    void SetCameraPtr(const Camera* _cameraPtr) { cameraptr_ = _cameraPtr; }
    void SetColor(const Vector4& _color) { color_ = _color; }
    void RegisterPoint(const Vector3& _start, const Vector3& _end);
    void Draw();

    void DrawOBB(const Matrix4x4& _affineMat);
    void DrawOBB(const std::array <Vector3, 8>& _vertices);
    void DrawSphere(const Matrix4x4& _affineMat);

private:
    void TransferData();

    const uint32_t kMaxNum = 4096u;
    uint32_t index = 0u;
    Vector4 color_ = { 0,0,0,1 };
    const Camera* cameraptr_=nullptr;
    ID3D12RootSignature* rootSignature_ = nullptr;
    ID3D12PipelineState* graphicsPipelineState_ = nullptr;

    struct ConstantBufferData
    {
        Matrix4x4 vp;
        Vector4 color;
    };
    Microsoft::WRL::ComPtr <ID3D12Resource> resources_ = nullptr;
    ConstantBufferData* constMap_;

    struct PointData
    {
        Vector4 position;
    };

    PointData* vConstMap_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource>      vertexResource_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW                    vertexBufferView_ = {};

    void SetVerties();

    std::array <Vector3, 8> obbVertices_;
    std::array <uint32_t, 24> obbIndices_;
    std::array <Vector3, 32> circleVertices_;

    const float kDivision = 16;
    std::vector <Vector3> sphereVertices_;
    std::vector <uint32_t> sphereIndices_;

    LineDrawer() = default;
    ~LineDrawer() = default;
    LineDrawer(const LineDrawer& ) = delete;
    LineDrawer& operator=(const LineDrawer&) = delete;
};