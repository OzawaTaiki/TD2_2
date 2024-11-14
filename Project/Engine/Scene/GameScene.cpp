#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include <chrono>
#include <imgui.h>

std::unique_ptr<BaseScene> GameScene::Create()
{
    return std::make_unique<GameScene>();
}

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


    // フォローカメラ
    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize();


    // プレイヤ
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera(&followCamera_->GetCamera());

    followCamera_->SetTarget(&player_->GetWorldTransform());

    // 敵
    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize();
    enemy_->SetPlayer(player_.get());

    // ステージ
    stage_ = std::make_unique<Stage>();
    stage_->Initialize();
    

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

    // ステージ
    stage_->Update();




    if (enemy_->GetBehavior() == Enemy::Behavior::kRoot) {

        // 追従カメラの更新
        followCamera_->Update();
        camera_->matView_ = followCamera_->GetCamera().matView_;
        camera_->matProjection_ = followCamera_->GetCamera().matProjection_;
    }
    else if (enemy_->GetBehavior() == Enemy::Behavior::kAttack) {
        camera_->matView_ = enemy_->GetCamera().matView_;
        camera_->matProjection_ = enemy_->GetCamera().matProjection_;
    }
    else if (enemy_->GetBehavior() == Enemy::Behavior::kAttack2) {
        camera_->matView_ = enemy_->GetCamera2().matView_;
        camera_->matProjection_ = enemy_->GetCamera2().matProjection_;
    }

   
    //camera_->UpdateMatrix();
    camera_->TransferData();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDrawForObjectModel();
    //<------------------------
    // プレイヤー
    player_->Draw(*camera_);

    // 敵
    enemy_->Draw(*camera_);

    // モデル
    stage_->Draw(*camera_);



    //<------------------------

    ModelManager::GetInstance()->PreDrawForAnimationModel();
    //<------------------------

    //<------------------------

    Sprite::PreDraw();
    //<------------------------



    //<------------------------
    lineDrawer_->Draw();


}
