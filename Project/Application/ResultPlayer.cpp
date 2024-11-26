#include "ResultPlayer.h"
#include "ModelManager.h"

#include "ConfigManager.h"

void ResultPlayer::Initialize()
{
    model_ = std::make_unique<ObjectModel>();

    ConfigManager* config = ConfigManager::GetInstance();
    config->SetVariable("Result", "pmodelPath", &modelName_);
    config->SetVariable("Result", "pBaseScale", &baseScale_);
    config->SetVariable("Result", "pScaleRange", &scaleRange_);
    config->SetVariable("Result", "pScaleSpeed", &scaleSpeed_);
    config->SetVariable("Result", "pCurrentTime", &currentTime_);
    config->SetVariable("Result", "rotate", &model_->rotate_);

    model_->Initialize(modelName_);
    model_->scale_ = baseScale_;
}

void ResultPlayer::Update()
{
#ifdef _DEBUG
    ImGui();
#endif // _DEBUG

    Scaling();

    model_->Update();
}

void ResultPlayer::Draw(const Camera* _camera)
{
    model_->Draw(_camera, { 1,1,1,1 });
}

void ResultPlayer::Scaling()
{
    currentTime_ += 1.0f / 60.0f;

    model_->scale_.x = baseScale_.x + scaleRange_.x * sinf(currentTime_ * scaleSpeed_);
    model_->scale_.y = baseScale_.y - scaleRange_.y * sinf(currentTime_ * scaleSpeed_);
    model_->scale_.z = baseScale_.z + scaleRange_.z * sinf(currentTime_ * scaleSpeed_);

}

#ifdef _DEBUG
#include <imgui.h>
void ResultPlayer::ImGui()
{
    ImGui::SeparatorText("Player");
    ImGui::DragFloat3("baseScale", &baseScale_.x, 0.01f);
    ImGui::DragFloat3("scaleRange", &scaleRange_.x, 0.01f);
    ImGui::DragFloat("scaleSpeed", &scaleSpeed_, 0.01f);

    char buf[255];
    strcpy_s(buf, modelName_.c_str());
    if(ImGui::InputText("modelPath", buf,255))
        modelName_ = buf;
    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData("Result");
        model_->SetModel(modelName_);
    }


}
#endif // _DEBUG
