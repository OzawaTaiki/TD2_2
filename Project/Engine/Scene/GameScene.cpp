#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "ParticleManager.h"
#include "TextureManager.h"
#include "ConfigManager.h"
#include <chrono>
#include <imgui.h>

GameScene::~GameScene()
{
    delete color_;
}

void GameScene::Initialize()
{
    input_ = Input::GetInstance();

    camera_ = std::make_unique<Camera>();
    camera_->Initialize();

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());


    audio_ = std::make_unique<Audio>();
    audio_->Initialize();

    model_ = Model::CreateFromObj("bunny.gltf");
    trans_.Initialize();
    trans_.UpdateData();
    color_ = new ObjectColor;
    color_->Initialize();

    ConfigManager::GetInstance()->LoadData();
    ConfigManager::GetInstance()->SetVariable("Model", "Position", &trans_.transform_);
    ConfigManager::GetInstance()->SetVariable("Model", "scale", &trans_.scale_);
    ConfigManager::GetInstance()->SetVariable("Model", "rotate", &trans_.rotate_);
}

void GameScene::Update()
{
    //ImGui::ShowDemoWindow();
    ImGui::Begin("Engine");

    input_->Update();
    //<-----------------------
    camera_->Update();

    ImGui::DragFloat3("Position", &trans_.transform_.x, 0.1f);
    ImGui::DragFloat3("Scale", &trans_.scale_.x, 0.1f);
    ImGui::DragFloat3("Rotate", &trans_.rotate_.x, 0.1f);


    if(ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData();
    }
    trans_.UpdateData();

    camera_->UpdateMatrix();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDraw();
    //<------------------------
    model_->Draw(trans_, camera_.get(), color_);

    //<------------------------


    Sprite::PreDraw();
    //<------------------------



    //<------------------------
    lineDrawer_->Draw();


}
