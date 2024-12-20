#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "ParticleManager.h"
#include "../Collider/Collider.h"
#include "../Collider/CollisionManager.h"

#include <chrono>
#include "ImGuiManager.h"
#include <SceneManager.h>

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

    // ステージ
    stage_ = std::make_unique<Stage>();
    stage_->Initialize();




    debugCamera_ = std::make_unique<DebugCamera>();
    debugCamera_->Initialize();

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());

    audio_ = std::make_unique<Audio>();
    audio_->Initialize();

    // 移動音
    gameBgm_.soundDataHandle = audio_->SoundLoadWave("resources/Sounds/gameBgm.wav");
    gameBgm_.volume = 0.08f;
    gameBgm_.voiceHandle = audio_->SoundPlay(gameBgm_.soundDataHandle, gameBgm_.volume, 1, 0);

    
    // フォローカメラ
    followCamera_ = std::make_unique<FollowCamera>();
    followCamera_->Initialize();


    // プレイヤ
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetCamera(&followCamera_->GetCamera());
    player_->SetStage(stage_.get());

    followCamera_->SetTarget(&player_->GetWorldTransform());

    // 敵
    enemy_ = std::make_unique<Enemy>();
    enemy_->Initialize();
    enemy_->SetPlayer(player_.get());
    enemy_->SetStage(stage_.get());
    enemy_->SetF(followCamera_.get());

    followCamera_->SetEnemy(enemy_.get());

    stage_->SetEnemy(enemy_.get());

    player_->SetEnemy(enemy_.get());

    ParticleManager::GetInstance()->Initialize();

    // ui
    ui_ = std::make_unique<GameSceneUI>();
    ui_->Initialize();

    lightGroup_ = std::make_unique<LightGroup>();
    lightGroup_->Initialize();
    lightGroup_->GetSpotLight()->SetIntensity(0);
    lightGroup_->GetPointLight()->SetIntensity(0);
    lightGroup_->GetDirectoinalLight()->SetIntensity(0.4f);
    lightGroup_->GetDirectoinalLight()->SetColor({ 0.607f,0.611f,0.8f,1 });

    player_->SetLight(lightGroup_.get());
    enemy_->SetLight(lightGroup_.get());
    stage_->SetLight(lightGroup_.get());
}

void GameScene::Update()
{
#ifdef _DEBUG
    if (input_->IsKeyPressed(DIK_RSHIFT) && Input::GetInstance()->IsKeyTriggered(DIK_RETURN))
    {
        activeDebugCamera_ = !activeDebugCamera_;
    }
#endif // _DEBUG

    CollisionManager::GetInstance()->ResetColliderList();


    //<-----------------------
    camera_->Update(0);
    // プレイヤー
    if(!activeDebugCamera_)
        player_->Update();

    // 敵
    enemy_->Update();

    // ステージ
    stage_->Update();



    if (activeDebugCamera_)
    {
        debugCamera_->Update();
        camera_->matView_ = debugCamera_->matView_;
        camera_->TransferData();
    }
   /* else if (enemy_->GetBehavior() == Enemy::Behavior::kAttack && enemy_->GetSpecialAttack() == Enemy::SpecialAttack::kAttack2) {
        followCamera_->Update();
        followCamera_->SetRotateY(0);
        camera_->matView_ = enemy_->GetCamera2().matView_;
        camera_->matProjection_ = enemy_->GetCamera2().matProjection_;
    }*/
    else {
        // 追従カメラの更新
        followCamera_->Update();
        camera_->matView_ = followCamera_->GetCamera().matView_;
        camera_->matProjection_ = followCamera_->GetCamera().matProjection_;
    }


    //camera_->UpdateMatrix();
    camera_->TransferData();


    // 追従カメラの更新
    followCamera_->Update();
    ParticleManager::GetInstance()->Update(camera_.get());
    CollisionManager::GetInstance()->CheckAllCollision();

    ui_->Update(player_->GetHPRatio(), enemy_->GetHPRatio());
    //<-----------------------

    if (enemy_->isPostKillEffectFinished())
    {
        timer_ += 1.0f / 60.0f;
        if (timer_ > fadeStartDelay_)
        {
            SceneManager::ReserveScene("gameclear");
        }
    }
    else if (player_->IsPostDieEffectFinished())
    {
        timer_ += 1.0f / 60.0f;
        if (timer_ > fadeStartDelay_)
        {
            SceneManager::ReserveScene("gameover");
        }
    }

    if (enemy_->GetBehavior() == Enemy::Behavior::kDie) {
        audio_->SoundStop(gameBgm_.voiceHandle);
    }

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


    ParticleManager::GetInstance()->Draw(camera_.get());
    //<------------------------

    Sprite::PreDraw();
    //<------------------------

    ui_->Draw();

    //<------------------------
    lineDrawer_->Draw();


}
