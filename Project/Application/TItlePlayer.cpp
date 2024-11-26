#include "TItlePlayer.h"
#include "ModelManager.h"

#include "ConfigManager.h"

void TItlePlayer::Initialize()
{
    model_ = std::make_unique<ObjectModel>();
    model_->Initialize("playerBody/playerbody.obj");

    ConfigManager::GetInstance()->SetVariable("Title", "pbasePos_", &basePos_);
    ConfigManager::GetInstance()->SetVariable("Title", "pscale", &model_->scale_);
    ConfigManager::GetInstance()->SetVariable("Title", "protate", &model_->rotate_);

    ConfigManager::GetInstance()->SetVariable("Title", "prange", &range_);
    ConfigManager::GetInstance()->SetVariable("Title", "pspeed", &speed_);

    model_->translate_ = basePos_;
}

void TItlePlayer::Update()
{
    Floating();


    model_->Update();
#ifdef _DEBUG
    ImGui();
#endif // _DEBUG
}

void TItlePlayer::Draw(const Camera* _camera)
{
    model_->Draw(_camera, { 1,1,1,1 });
}

void TItlePlayer::Floating()
{
    currentTime_ += 1.0f / 60.0f;

    model_->translate_.y = basePos_.y + range_ * sinf(currentTime_ * speed_);
}

#ifdef _DEBUG
#include <imgui.h>
void TItlePlayer::ImGui()
{

    ImGui::PushID(this);
    if (ImGui::DragFloat3("basePos", &basePos_.x, 0.01f))
    {
        model_->translate_.x = basePos_.x;
        model_->translate_.z = basePos_.z;
    }
    ImGui::DragFloat("range", &range_, 0.01f);
    ImGui::DragFloat("speed", &speed_, 0.01f);

    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData("Title");
    }
    ImGui::PopID();
}
#endif // _DEBUG
