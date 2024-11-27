#include "GameOverScene.h"

#include "SceneManager.h"
#include "Input.h"
std::unique_ptr<BaseScene>GameOverScene::Create()
{
    return std::make_unique<GameOverScene>();
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize()
{
    camera_.Initialize();
    camera_.translate_ = { 0,10,-30 };
    camera_.rotate_ = { 0.26f,0,0 };

    enemy_ = std::make_unique<ResultEnemy>();
    enemy_->Initialize();

    ui_.Initialize("GameOver");


    //音
    audio_ = std::make_unique<Audio>();
    audio_->Initialize();
    audio2_ = std::make_unique<Audio>();
    audio2_->Initialize();
    // 移動音
    gameOverBgm_.soundDataHandle = audio_->SoundLoadWave("resources/Sounds/gameOverBgm.wav");
    gameOverBgm_.volume = 0.2f;
    gameOverBgm_.voiceHandle = audio_->SoundPlay(gameOverBgm_.soundDataHandle, gameOverBgm_.volume, 1, 0);

    decision.soundDataHandle = audio2_->SoundLoadWave("resources/Sounds/decision.wav");
    decision.volume = 0.2f;
   // choice.voiceHandle = audio2_->SoundPlay(choice.soundDataHandle, choice.volume, 0, 0);

    lightGroup_ = std::make_unique<LightGroup>();
    lightGroup_->Initialize();
    lightGroup_->GetSpotLight()->SetIntensity(0);
    lightGroup_->GetPointLight()->SetIntensity(0);
    lightGroup_->GetDirectoinalLight()->SetIntensity(0.4f);
    lightGroup_->GetDirectoinalLight()->SetColor({ 0.607f,0.611f,0.8f,1 });

    enemy_->SetLight(lightGroup_.get());

}

void GameOverScene::Update()
{
    enemy_->Update();

    ui_.Update();

    camera_.Update();
    camera_.UpdateMatrix();

    if (Input::GetInstance()->IsKeyTriggered(DIK_SPACE) ||
        Input::GetInstance()->IsPadTriggered(PadButton::iPad_A))
    {
        uint32_t select = ui_.GetSelectButton();

        if (select == 0)
        {

            SceneManager::ReserveScene("title");
        }
        else if (select == 1)
        {
            SceneManager::ReserveScene("game");

        }
        audio_->SoundStop(gameOverBgm_.voiceHandle);

    }


}

void GameOverScene::Draw()
{
    ui_.DrawBackGround();

    enemy_->Draw(&camera_);

    ui_.Draw();

}

#ifdef _DEBUG
#include <imgui.h>
void GameOverScene::ImGui()
{

}
#endif // _DEBUG