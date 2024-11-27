#include "ResultEnemy.h"
#include "ModelManager.h"

#include "ConfigManager.h"

void ResultEnemy::Initialize()
{
    model_ = std::make_unique<ObjectModel>();

    ConfigManager* config = ConfigManager::GetInstance();
    config->SetVariable("GameOver", "emodelPath", &modelName_);
    config->SetVariable("GameOver", "eBaseScale", &baseScale_);
    config->SetVariable("GameOver", "eScaleRange", &scaleRange_);
    config->SetVariable("GameOver", "eScaleSpeed", &scaleSpeed_);
    config->SetVariable("GameOver", "eCurrentTime", &currentTime_);
    config->SetVariable("GameOver", "erotate", &model_->rotate_);
    config->SetVariable("GameOver", "ePosition", &model_->translate_);

    model_->Initialize(modelName_);
    model_->scale_ = baseScale_;
}

void ResultEnemy::Update()
{
#ifdef _DEBUG
    ImGui();
#endif // _DEBUG

    Scaling();

    model_->Update();
}

void ResultEnemy::Draw(const Camera* _camera)
{
    model_->Draw(_camera, { 1,1,1,1 });
}

void ResultEnemy::Scaling()
{
    currentTime_ += 1.0f / 60.0f;

    model_->scale_.x = baseScale_.x + scaleRange_.x * sinf(currentTime_ * scaleSpeed_);
    model_->scale_.y = baseScale_.y - scaleRange_.y * sinf(currentTime_ * scaleSpeed_);
    model_->scale_.z = baseScale_.z + scaleRange_.z * sinf(currentTime_ * scaleSpeed_);

}

#ifdef _DEBUG
#include <imgui.h>
void ResultEnemy::ImGui()
{
    ImGui::SeparatorText("Player");
    ImGui::DragFloat3("baseScale", &baseScale_.x, 0.01f);
    ImGui::DragFloat3("scaleRange", &scaleRange_.x, 0.01f);
    ImGui::DragFloat("scaleSpeed", &scaleSpeed_, 0.01f);

    char buf[255];
    strcpy_s(buf, modelName_.c_str());
    if (ImGui::InputText("modelPath", buf, 255))
        modelName_ = buf;
    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData("GameOver");
        model_->SetModel(modelName_);
    }


}
#endif // _DEBUG
