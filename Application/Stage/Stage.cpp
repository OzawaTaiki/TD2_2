#include "Stage.h"

void Stage::Initialize()
{
    // 床
    worldFloor_.Initialize();
    worldFloor_.transform_ = Vector3{ 0,-3.0f,0 };
    worldFloor_.rotate_.y = { 1.57f };

    // 床モデル
    model_ = Model::CreateFromObj("Stage/Stage.obj");



    color_.Initialize();
    color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void Stage::Update()
{
    worldFloor_.UpdateData();
}

void Stage::Draw(const Camera& camera)
{
    // モデル
    model_->Draw(worldFloor_, &camera, &color_);
}
