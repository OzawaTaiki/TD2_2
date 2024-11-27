#include "TItleUI.h"

#include "SpriteManager.h"
#include "ImGuiManager.h"
#include "ConfigManager.h"
#include "TextureManager.h"

void TitleUI::Initialize()
{
    pressAnyKey_ = Sprite::Create(0);
    pressAnyKey_->Initialize();

    title_ = std::make_unique<ObjectModel>();
    title_->Initialize(titleModelpath_);

    ConfigManager* config = ConfigManager::GetInstance();
    config->SetVariable("Title", "UIpos", &pressAnyKey_->translate_);
    config->SetVariable("Title", "UIscale", &pressAnyKey_->scale_);
    config->SetVariable("Title", "UIrotate", &pressAnyKey_->rotate_);
    config->SetVariable("Title", "UIcolor", &pressTextColor_);
    config->SetVariable("Title", "UITextTexture", &textTextureParh_);
    config->SetVariable("Title", "UItitleModel", &titleModelpath_);
    title_->translate_;
    // title_のｔらんｓふぉーむをとうろく
    config->SetVariable("Title", "titlePos", &title_->translate_);
    config->SetVariable("Title", "titleScale", &title_->scale_);
    config->SetVariable("Title", "titleRotate", &title_->rotate_);


    uint32_t handle = TextureManager::GetInstance()->Load(textTextureParh_);
    pressAnyKey_->SetTextureHandle(handle);

    title_->SetModel(titleModelpath_);
}

void TitleUI::Update()
{
    title_->Update();
    TextUpdate();

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
    ImGui::SeparatorText("Title");
    ImGui::PushID("Title");
    ImGui::DragFloat3("Position", &title_->translate_.x, 0.01f);
    ImGui::DragFloat3("Scale", &title_->scale_.x, 0.01f);
    ImGui::DragFloat3("Rotate", &title_->rotate_.x, 0.01f);
    if (ImGui::InputText("TitleModel", buf2, sizeof(buf2)))
    {
        titleModelpath_ = buf2;
    }
    ImGui::PopID();
    if (ImGui::SmallButton("Set"))
    {
        pressAnyKey_->SetTextureHandle(TextureManager::GetInstance()->Load(textTextureParh_));
        title_->SetModel(titleModelpath_);
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

void TitleUI::TextUpdate()
{
    currentTime_ += 1.0f / 60.0f;
    if (currentTime_ >= cycle_)
    {
        currentTime_ = 0.0f;
    }
    float t = currentTime_ / cycle_;

    float n1 = 7.5625f;
    float d1 = 2.75f;

    float easedT = 0.0f;
    if (t < 1.0f / d1) {
        easedT= n1 * t * t;
    }
    else if (t < 2 / d1) {
        easedT = n1 * (t -= 1.5f / d1) * t + 0.8f;
    }
    else if (t < 2.5 / d1) {
        easedT = n1 * (t -= 2.25f / d1) * t + 0.9375f;
    }
    else {
        easedT = n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }

    if (easedT < threshold_)
    {
        pressTextColor_.w = 0;
    }
    else
    {
        pressTextColor_.w = 1;
    }
}
