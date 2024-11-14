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
    camera_->rotate_ = Vector3{ 0.34f,0,0 };


    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());

    audio_ = std::make_unique<Audio>();
    audio_->Initialize();


    worldTransform.Initialize();
    worldTransform.transform_ = Vector3{ 0,-3.0f,0 };
    worldTransform.rotate_.y = { 1.57f };

    model_ = Model::CreateFromObj("Stage/Stage.obj");


    color_.Initialize();
    color_.SetColor(Vector4{ 1, 1, 1, 1 });

    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize();

    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera(&followCamera_->GetCamera());

    followCamera_->SetTarget(&player_->GetWorldTransform());
    

    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize();
    enemy_->SetPlayer(player_.get());
}

void GameScene::Update()
{
    ImGui::Begin("Engine");

    input_->Update();
    //<-----------------------
    camera_->Update();

    // プレイヤー
    player_->Update();

    // 敵
    enemy_->Update();

    worldTransform.UpdateData();


    // 追従カメラの更新
    followCamera_->Update();
    
    if (enemy_->GetBehavior() == Enemy::Behavior::kRoot) {
        camera_->matView_ = followCamera_->GetCamera().matView_;
        camera_->matProjection_ = followCamera_->GetCamera().matProjection_;
    }
    else {
        camera_->matView_ = enemy_->GetCamera().matView_;
        camera_->matProjection_ = enemy_->GetCamera().matProjection_;
    }



    camera_->TransferData();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDraw();
    //<------------------------

    // プレイヤー
    player_->Draw(*camera_);

    // 敵
    enemy_->Draw(*camera_);

    // モデル
    model_->Draw(worldTransform, camera_.get(), &color_);

    //<------------------------
    Sprite::PreDraw();
    //<------------------------



    //<------------------------

    lineDrawer_->Draw();


}
