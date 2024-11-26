#include "TItleUI.h"

#include "SpriteManager.h"
#include "ImGuiManager.h"
#include "ConfigManager.h"
#include "TextureManager.h"

void TitleUI::Initialize()
{
    pressAnyKey_ = Sprite::Create(0);
    pressAnyKey_->Initialize();

    ConfigManager* config = ConfigManager::GetInstance();
    config->SetVariable("TitleUI", "pos", &pressAnyKey_->translate_);
    config->SetVariable("TitleUI", "scale", &pressAnyKey_->scale_);
    config->SetVariable("TitleUI", "rotate", &pressAnyKey_->rotate_);
    config->SetVariable("TitleUI", "color", &pressTextColor_);
    config->SetVariable("TitleUI", "TextTexture", &textTextureParh_);
    config->SetVariable("TitleUI", "titleModel", &titleModelpath_);

    uint32_t handle = TextureManager::GetInstance()->Load(textTextureParh_);
    pressAnyKey_->SetTextureHandle(handle);

    title_ = std::make_unique<ObjectModel>();
    title_->Initialize(titleModelpath_);
}

void TitleUI::Update()
{
    title_->Update();

#ifdef _DEBUG

    ImGui::SeparatorText("PressText");
    ImGui::DragFloat2("Position", &pressAnyKey_->translate_.x, 1);
    ImGui::DragFloat2("Scale", &pressAnyKey_->scale_.x, 0.01f);
    ImGui::DragFloat("Rotate", &pressAnyKey_->rotate_, 0.01f);
    ImGui::ColorEdit4("Color", &pressTextColor_.x);
    char buf1[255];
    strcpy_s(buf1, textTextureParh_.c_str());
    char buf2[255];
    strcpy_s(buf2, titleModelpath_.c_str());

    ImGui::SeparatorText("path");
    if (ImGui::InputText("TextTexture", buf1, sizeof(buf1)))
    {
        textTextureParh_ = buf1;
    }
    if (ImGui::InputText("TitleModel", buf2, sizeof(buf2)))
    {
        titleModelpath_ = buf2;
    }

#endif // _DEBUG

}

void TitleUI::Draw(const Camera* _camera)
{
    title_->Draw(_camera, { 1,1,1,1 });


    SpriteManager::GetInstance()->PreDraw();
    pressAnyKey_->Draw(pressTextColor_);
}

void TitleUI::Save()
{
    title_->SetModel(titleModelpath_);
    pressAnyKey_->SetTextureHandle(TextureManager::GetInstance()->Load(textTextureParh_));
}
