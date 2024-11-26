#include "GameClearScene.h"

#include "SceneManager.h"

std::unique_ptr<BaseScene>GameClearScene::Create()
{
    return std::make_unique<GameClearScene>();
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Initialize()
{
}

void GameClearScene::Update()
{
}

void GameClearScene::Draw()
{
}

#ifdef _DEBUG
#include <imgui.h>
void GameClearScene::ImGui()
{

}
#endif // _DEBUG