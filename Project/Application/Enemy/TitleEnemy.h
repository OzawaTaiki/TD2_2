#pragma once

#include "WorldTransform.h"
#include "ObjectModel.h"
#include "Camera.h"

class TitleEnemy
{
public:

    TitleEnemy() = default;
    ~TitleEnemy() = default;

    void Initialize();
    void Update();
    void Draw(const Camera* _camera);

    void SetLight(LightGroup* _ptr) { model_->SetLightGroup(_ptr); }

private:
    std::unique_ptr<ObjectModel> model_ = nullptr;

    float currentTime_ = 0.0f;
    // 上下の幅
    float range_ = 0.0f;
    // 上下の速度
    float speed_ = 0.0f;
    // 基準の高さ
    Vector3 basePos_ = 0.0f;

    void Floating();

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG

};