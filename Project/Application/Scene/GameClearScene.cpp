#include "GameClearScene.h"

#include "SceneManager.h"
#include "Input.h"
std::unique_ptr<BaseScene>GameClearScene::Create()
{
    return std::make_unique<GameClearScene>();
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Initialize()
{
    camera_.Initialize();
    camera_.translate_ = { 0,10,-30 };
    camera_.rotate_ = { 0.26f,0,0 };

    player_ = std::make_unique<ResultPlayer>();
    player_->Initialize();

    ui_.Initialize("Clear");


    audio_ = std::make_unique<Audio>();
    audio_->Initialize();
    audio2_ = std::make_unique<Audio>();
    audio2_->Initialize();

    // 移動音
    clearBgm_.soundDataHandle = audio_->SoundLoadWave("resources/Sounds/clearBgm.wav");
    clearBgm_.volume = 0.08f;
    clearBgm_.voiceHandle = audio_->SoundPlay(clearBgm_.soundDataHandle, clearBgm_.volume, 1, 0);
    decision.soundDataHandle = audio2_->SoundLoadWave("resources/Sounds/decision.wav");
    decision.volume = 0.2f;
}

void GameClearScene::Update()
{
    player_->Update();

    ui_.Update();

    camera_.Update();
    camera_.UpdateMatrix();

    if (Input::GetInstance()->IsKeyTriggered(DIK_SPACE) ||
        Input::GetInstance()->IsPadTriggered(PadButton::iPad_A))
    {
        uint32_t select = ui_.GetSelectButton();

        if (select == 0)
        {
            SceneManager::ReserveScene("game");
            decision.voiceHandle = audio2_->SoundPlay(decision.soundDataHandle, decision.volume, 0, 0);
        }
        else if (select == 1)
        {
            SceneManager::ReserveScene("title");
            decision.voiceHandle = audio2_->SoundPlay(decision.soundDataHandle, decision.volume, 0, 0);
        }
        audio_->SoundStop(clearBgm_.voiceHandle);
    }


}

void GameClearScene::Draw()
{
    ui_.DrawBackGround();

    player_->Draw(&camera_);

    ui_.Draw();

}

#ifdef _DEBUG
#include <imgui.h>
void GameClearScene::ImGui()
{

}
#endif // _DEBUG