#include "Stage.h"

void Stage::Initialize()
{
    // 床
    worldFloor_.Initialize();
    worldFloor_.transform_ = Vector3{ 0,-3.0f,0 };
    worldFloor_.rotate_.y = { 1.57f };

    // 床モデル
    modelFloor_ = Model::CreateFromObj("Tile/Tile.gltf");

    // 壁(前)
    worldWallFlont.Initialize();
    worldWallFlont.transform_ = Vector3{ 0,0,-100 };
    worldWallFlont.rotate_ = Vector3{ 0,0,0};
    
    worldWallBack.Initialize();
    worldWallBack.transform_ = Vector3{ 0,0,100 };
    worldWallBack.rotate_ = Vector3{ 0,-3.14,0 };

    worldWallRight.Initialize();
    worldWallRight.transform_ = Vector3{ 100,0,0 };
    worldWallRight.rotate_ = Vector3{ 0,DegreesToRadians(-90),0};

    worldWallLeft.Initialize();
    worldWallLeft.transform_ = Vector3{ -100,0,0 };
    worldWallLeft.rotate_ = Vector3{ 0,DegreesToRadians(90),0 };

    modelWallFlont_ = Model::CreateFromObj("wall/wall.obj");

    color_.Initialize();
    color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void Stage::Update()
{
    if (ImGui::BeginTabBar("GameScene"))
    {
        if (ImGui::BeginTabItem("stage"))
        {
            

            ImGui::DragFloat3("translate", &worldWallLeft.transform_.x, 0.01f);
            ImGui::DragFloat3("rotate", &worldWallLeft.rotate_.x, 0.01f);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }


   worldWallFlont.UpdateData();

    worldFloor_.UpdateData();
    worldWallBack.UpdateData();
    worldWallLeft.UpdateData();
    worldWallRight.UpdateData();
}

void Stage::Draw(const Camera& camera)
{
    // モデル
    modelFloor_->Draw(worldFloor_, &camera, &color_);

    // 壁(前)
    modelWallFlont_->Draw(worldWallFlont, &camera, &color_);
    modelWallFlont_->Draw(worldWallBack, &camera, &color_);
    modelWallFlont_->Draw(worldWallLeft, &camera, &color_);
    modelWallFlont_->Draw(worldWallRight, &camera, &color_);
}