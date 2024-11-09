#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include <chrono>
#include <imgui.h>

GameScene::~GameScene()
{
    delete color_;
    delete model_;
    delete humanModel_;
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

    model_ = new ObjectModel;
    model_->Initialize("human/sneakWalk.gltf");
    humanModel_ = new ObjectModel;
    humanModel_->Initialize("human/walk.gltf");

}

void GameScene::Update()
{
    //ImGui::ShowDemoWindow();
    ImGui::Begin("Engine");

    input_->Update();
    //<-----------------------
    camera_->Update();


    model_->Update();
    humanModel_->Update();

    camera_->TransferData();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDraw();
    //<------------------------
    model_->Draw(camera_.get(), { 1,1,1,1 });
    humanModel_->Draw(camera_.get(), { 1,1,1,1 });

    //<------------------------


    Sprite::PreDraw();
    //<------------------------



    //<------------------------
    lineDrawer_->Draw();


}
