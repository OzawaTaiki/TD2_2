#include "TitleScene.h"

#include "SceneManager.h"

std::unique_ptr<BaseScene>TitleScene::Create()
{
    return std::make_unique<BaseScene>();
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
}

void TitleScene::Update()
{
}

void TitleScene::Draw()
{
}

#ifdef _DEBUG
#include <imgui.h>
void TitleScene::ImGui()
{

}
#endif // _DEBUG