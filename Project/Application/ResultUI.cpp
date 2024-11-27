#include "ResultUI.h"
#include "ModelManager.h"
#include "Input.h"
#include "ConfigManager.h"
#include "TextureManager.h"

void ResultUI::Initialize(const std::string& _scene)
{
    scene_ = _scene;
    std::string label1;
    std::string label2;

    if (_scene == "Clear")
    {
        label1 = "Retry";
        label2 = "Title";
    }
    else if (_scene == "GameOver")
    {
        label1 = "continue";
        label2 = "Title";
    }

    retryButton_ = std::make_unique<UIButton>();
    retryButton_->Initialize(label1);
    retryButton_->SetAnchor({ 0.5f,0.5f });


    titleButton_ = std::make_unique<UIButton>();
    titleButton_->Initialize(label2);
    titleButton_->SetAnchor({ 0.5f,0.5f });

    baseSize_ = titleButton_->GetSize();

    ConfigManager* configManager = ConfigManager::GetInstance();
    configManager->SetVariable(  _scene, "baseSize", &baseSize_);
    configManager->SetVariable(  _scene, "retrySize", &retrySize_);
    configManager->SetVariable(  _scene, "titleSize", &titleSize_);
    configManager->SetVariable(  _scene, "sizeRange", &sizeRange_);
    configManager->SetVariable(  _scene, "resizeSpeed", &resizeSpeed_);

    configManager->SetVariable(  _scene, "backGroundTexturePath", &backGroundTexturePath_);
    configManager->SetVariable(  _scene, "textTexturePath", &textTexturePath_);
    configManager->SetVariable(  _scene, "retryButtonTexturePath", &retryButtonTexturePath_);
    configManager->SetVariable(  _scene, "titleButtonTexturePath", &titleButtonTexturePath_);

    retryButton_->SetTextureNameAndLoad(retryButtonTexturePath_);
    titleButton_->SetTextureNameAndLoad(titleButtonTexturePath_);


    backGround_ = Sprite::Create(TextureManager::GetInstance()->Load(backGroundTexturePath_), { 0,0 });
    backGround_->Initialize();
    backGround_->SetSize({ 1280,720 });

    text_ = Sprite::Create(TextureManager::GetInstance()->Load(textTexturePath_));
    text_->Initialize();
    configManager->SetVariable(  _scene, "textSize", &textSize_);
    configManager->SetVariable(  _scene, "textBasePos", &textBasePos_);
    configManager->SetVariable(  _scene, "textfloatingSpeed", &textfloatingSpeed_);
    configManager->SetVariable(  _scene, "textfloatingRange", &textfloatingRange_);

    audio_ = std::make_unique<Audio>();
    audio_->Initialize();

    choice.soundDataHandle = audio_->SoundLoadWave("resources/Sounds/choice.wav");
    choice.volume = 0.2f;
}

void ResultUI::Update()
{
#ifdef _DEBUG
    ImGui::Begin("UI");
    ImGui::SeparatorText("Button");
    ImGui::DragFloat("resizeSpeed", &resizeSpeed_, 0.01f) ;
    ImGui::DragFloat2("sizeRange", &sizeRange_.x, 0.1f);
    ImGui::DragFloat2("baseSize", &baseSize_.x,1);

    ImGui::SeparatorText("text");
    ImGui::DragFloat2("textSize", &textSize_.x, 1);
    ImGui::DragFloat2("textBasePos", &textBasePos_.x, 1);
    ImGui::DragFloat("textfloatingSpeed", &textfloatingSpeed_, 0.01f);
    ImGui::DragFloat("textfloatingRange", &textfloatingRange_, 0.01f);

    ImGui::SeparatorText("Path");
    char buf[256];
    strcpy_s(buf, backGroundTexturePath_.c_str());
    if (ImGui::InputText("backGroundTexturePath", buf, sizeof(buf)))
    {
        backGroundTexturePath_ = buf;
    }
    char buf2[256];
    strcpy_s(buf2, textTexturePath_.c_str());
    if (ImGui::InputText("textTexturePath", buf2, sizeof(buf2)))
    {
        textTexturePath_ = buf2;
    }
    char buf3[256];
    strcpy_s(buf3, retryButtonTexturePath_.c_str());
    if (ImGui::InputText("retryButtonTexturePath", buf3, sizeof(buf3)))
    {
        retryButtonTexturePath_ = buf3;
    }
    char buf4[256];
    strcpy_s(buf4, titleButtonTexturePath_.c_str());
    if (ImGui::InputText("titleButtonTexturePath", buf4, sizeof(buf4)))
    {
        titleButtonTexturePath_ = buf4;
    }
    if (ImGui::Button("load"))
    {
        backGround_->SetTextureHandle(TextureManager::GetInstance()->Load(backGroundTexturePath_));
        text_->SetTextureHandle(TextureManager::GetInstance()->Load(textTexturePath_));
        retryButton_->SetTextureNameAndLoad(retryButtonTexturePath_);
        titleButton_->SetTextureNameAndLoad(titleButtonTexturePath_);
    }

    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData(  scene_);
    }
    ImGui::End();
#endif // _DEBUG

    ButtonUpdate();

    currentTextTimer_ += 1.0f / 60.0f;
    float t = sinf(currentTextTimer_ * textfloatingSpeed_);
    Vector2 pos = textBasePos_ + Vector2{ 0,textfloatingRange_ } *t;
    text_->translate_ = pos;
    text_->SetSize(textSize_);


}

void ResultUI::Draw()
{
    Sprite::PreDraw();

    text_->Draw();

    retryButton_->Draw();
    titleButton_->Draw();
}

void ResultUI::DrawBackGround()
{
    Sprite::PreDraw();
    backGround_->Draw();
}

void ResultUI::ButtonUpdate()
{
    retryButton_->Update();
    titleButton_->Update();

    if (Input::GetInstance()->IsControllerConnected())
    {
        if (Input::GetInstance()->IsPadPressed(PadButton::iPad_Left) ||
            Input::GetInstance()->IsPadPressed(PadButton::iPad_Right))
        {
            choice.voiceHandle = audio_->SoundPlay(choice.soundDataHandle, choice.volume, 0, 1);
            selectButton_ = (selectButton_ + 1) % buttunNum_;
        }
    }
    if (Input::GetInstance()->IsKeyTriggered(DIK_LEFTARROW) ||
        Input::GetInstance()->IsKeyTriggered(DIK_RIGHTARROW) ||
        Input::GetInstance()->IsKeyTriggered(DIK_A) ||
        Input::GetInstance()->IsKeyTriggered(DIK_D))
    {
        choice.voiceHandle = audio_->SoundPlay(choice.soundDataHandle, choice.volume, 0, 1);
        selectButton_ = selectButton_ == 0 ? 1 : 0;
    }

    bool active = selectButton_;
    retryButton_->SetActive(active);
    titleButton_->SetActive(!active);

    if (retryButton_->IsActive())
    {
        currentRetryTimer_ += 1.0f / 60.0f;
        currentTitleTimer_ -= 1.0f / 10.0f;
    }
    if (titleButton_->IsActive())
    {
        currentTitleTimer_ += 1.0f / 60.0f;
        currentRetryTimer_ -= 1.0f / 10.0f;
    }

    if (currentRetryTimer_ < 0)
        currentRetryTimer_ = 0;
    if (currentTitleTimer_ < 0)
        currentTitleTimer_ = 0;

    float retryT = sinf(currentRetryTimer_ * resizeSpeed_);

    retrySize_ = baseSize_ + sizeRange_ * retryT;
    if (std::fabsf(retryT) <= 0.001f)
    {
        retrySize_ = baseSize_;
        currentRetryTimer_ = 0;
    }
    retryButton_->SetSize(retrySize_);

    float titleT = sinf(currentTitleTimer_ * resizeSpeed_);
    titleSize_ = baseSize_ + sizeRange_ * titleT;

    if (std::fabsf(titleT) <= 0.001f)
    {
        titleSize_ = baseSize_;
        currentTitleTimer_ = 0;
    }

    titleButton_->SetSize(titleSize_);
}

#ifdef _DEBUG
#include <imgui.h>
void ResultUI::ImGui()
{

}
#endif // _DEBUG
