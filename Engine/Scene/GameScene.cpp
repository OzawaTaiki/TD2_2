#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include <chrono>
#include <imgui.h>

GameScene::~GameScene()
{

}

void GameScene::Initialize()
{
    input_ = Input::GetInstance();

    camera_ = std::make_unique<Camera>();
    camera_->Initialize();
    camera_->translate_ = Vector3{ 0,18,-50 };
    camera_->rotate_ = Vector3{ 0.30f,0,0 };


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());

    audio_ = std::make_unique<Audio>();
    audio_->Initialize();


    worldTransform.Initialize();
    worldTransform.transform_ = Vector3{ 0,0,0 };
    worldTransform.rotate_.y = { 1.57f };

    model_ = Model::CreateFromObj("Box/Box.obj");


    color_.Initialize();
    color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void GameScene::Update()
{
    ImGui::Begin("Engine");

    input_->Update();
    //<-----------------------
    camera_->Update();

    worldTransform.UpdateData();

    camera_->UpdateMatrix();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDraw();
    //<------------------------


    model_->Draw(worldTransform, camera_.get(), &color_);

    //<------------------------
    Sprite::PreDraw();
    //<------------------------



    //<------------------------

    lineDrawer_->Draw();


}
