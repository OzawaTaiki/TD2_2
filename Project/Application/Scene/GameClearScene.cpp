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
        }
        else if (select == 1)
        {
            SceneManager::ReserveScene("title");
        }
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