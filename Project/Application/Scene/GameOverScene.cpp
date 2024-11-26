#include "GameOverScene.h"

#include "SceneManager.h"

std::unique_ptr<BaseScene>GameOverScene::Create()
{
    return std::make_unique<GameOverScene>();
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize()
{
}

void GameOverScene::Update()
{
}

void GameOverScene::Draw()
{
}

#ifdef _DEBUG
#include <imgui.h>
void GameOverScene::ImGui()
{

}
#endif // _DEBUG