#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "../Collider/Collider.h"
#include "../Collider/CollisionManager.h"

#include <chrono>
#include <imgui.h>

std::unique_ptr<BaseScene> GameScene::Create()
{
    return std::make_unique<GameScene>();
}

GameScene::~GameScene()
{
    delete color_;
    delete model_;
    delete humanModel_;
    delete emit_;
}

void GameScene::Initialize()
{

    input_ = Input::GetInstance();

    camera_ = std::make_unique<Camera>();
    camera_->Initialize();

    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize();


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());

    audio_ = std::make_unique<Audio>();
    audio_->Initialize();

    model_ = new ObjectModel;
    model_->Initialize("ColliderTestModel.obj");
    humanModel_ = new ObjectModel;
    humanModel_->Initialize("bunny.gltf");

}

void GameScene::Update()
{
    //ImGui::ShowDemoWindow();
    ImGui::Begin("Engine");
    input_->Update();
    CollisionManager::GetInstance()->ResetColliderList();

    if (input_->IsKeyPressed(DIK_RSHIFT) && Input::GetInstance()->IsKeyTriggered(DIK_RETURN))
    {
        activeDebugCamera_ = !activeDebugCamera_;
    }

    //<-----------------------


    model_->Update();
    humanModel_->Update();

    if (activeDebugCamera_)
    {
        debugCamera_->Update();
        camera_->matView_ = debugCamera_->matView_;
        camera_->TransferData();
    }
    else
    {
        camera_->Update();
        camera_->TransferData();
    }

    CollisionManager::GetInstance()->CheckAllCollision();

    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();
    //<------------------------
    model_->Draw(camera_.get(), { 1,1,1,1 });
    humanModel_->Draw(camera_.get(), { 1,1,1,1 });

    //<------------------------

    ModelManager::GetInstance()->PreDrawForAnimationModel();
    //<------------------------
    //humanModel_->Draw(camera_.get(), { 1,1,1,1 });

    //<------------------------

    Sprite::PreDraw();
    //<------------------------



    //<------------------------
    lineDrawer_->Draw();


}
