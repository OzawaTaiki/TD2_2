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