#include "Stage.h"

void Stage::Initialize()
{
    // 床
    worldFloor_.Initialize();
    worldFloor_.transform_ = Vector3{ 0,-3.0f,0 };
    worldFloor_.rotate_.y = { 1.57f };

    // 床モデル
    modelFloor_ = Model::CreateFromObj("Stage/Stage.obj");

    // 壁(前)
    worldWallFlont.Initialize();
    worldWallFlont.transform_ = Vector3{0,0,-30};
    worldWallBack.Initialize();
    worldWallBack.transform_ = Vector3{ 0,0,30 };
    worldWallRight.Initialize();
    worldWallRight.transform_ = Vector3{ 30,0,0 };
    worldWallLeft.Initialize();
    worldWallLeft.transform_ = Vector3{ -30,0,0 };



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
    modelFloor_->Draw(worldFloor_, &camera, &color_);
}
