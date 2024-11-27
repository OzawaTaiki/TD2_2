#include "GameSceneUI.h"
#include "TextureManager.h"
#include "ConfigManager.h"
#include "ImGuiManager.h"

#include <algorithm>

void GameSceneUI::Initialize()
{
    uint32_t frameHandle = TextureManager::GetInstance()->Load("PlayerHp.png");
    uint32_t baseHandle = TextureManager::GetInstance()->Load("PlayerHpGageBase.png");
    uint32_t barHandle = TextureManager::GetInstance()->Load("PlayerHpGage.png");

    playerHP_ = std::make_unique<HPUISprite>();
    playerHP_->Initialize(frameHandle, baseHandle, barHandle, "player");

    frameHandle = TextureManager::GetInstance()->Load("bossHp.png");
    baseHandle = TextureManager::GetInstance()->Load("bossHpGageBase.png");
    barHandle = TextureManager::GetInstance()->Load("bossHpGage.png");

    enemyHP_ = std::make_unique<HPUISprite>();
    enemyHP_->Initialize(frameHandle, baseHandle, barHandle, "enemy");

    Controler_ = Sprite::Create(TextureManager::GetInstance()->Load("gameAttack.png"));
    Controler_->Initialize();

    ConfigManager* configManager = ConfigManager::GetInstance();
    configManager->SetVariable("GameSceneUI", "cPos", &Controler_->translate_);
    configManager->SetVariable("GameSceneUI", "cSize", &Controler_->scale_);

}

void GameSceneUI::Update(float _playerHP, float _enemyHP)
{
#ifdef _DEBUG
    ImGui::Begin("GameUI");
    ImGui::BeginTabBar("GameUI");
    if (ImGui::BeginTabItem("controler"))
    {
        ImGui::DragFloat2("pos", &Controler_->translate_.x, 1);
        ImGui::DragFloat2("scale", &Controler_->scale_.x, 0.01f);
        ImGui::EndTabItem();
    }
#endif // _DEBUG
    playerHP_->Update(_playerHP);
    enemyHP_->Update(_enemyHP);

#ifdef _DEBUG
    ImGui::EndTabBar();
    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData("GameSceneUI");
    }
    ImGui::End();
#endif // _DEBUG
}

void GameSceneUI::Draw()
{
    playerHP_->Draw();
    enemyHP_->Draw();
    Controler_->Draw();
}

void HPUISprite::Initialize(uint32_t _frameHandle, uint32_t _baseHandle, uint32_t _barHandle, const std::string& _name)
{
    name_ = _name;

    frame_ = Sprite::Create(_frameHandle, { 0,0 });
    frame_->Initialize();

    base_ = Sprite::Create(_baseHandle, { 0,0 });
    base_->Initialize();

    hpBar_ = Sprite::Create(_barHandle, { 0,0});
    hpBar_->Initialize();

    baseSize_ = hpBar_->GetSize();
    barSize_  = hpBar_->GetSize();

    basePos_   = { 0,0 };
    barOffset_ = { 0,0 };

    ConfigManager* configManager = ConfigManager::GetInstance();
    configManager->SetVariable("GameSceneUI", _name + "basePos",    &basePos_);
    configManager->SetVariable("GameSceneUI", _name + "barOffset",  &barOffset_);
    configManager->SetVariable("GameSceneUI", _name + "baseSize",   &baseSize_);

    frame_->SetSize(baseSize_);
    base_ ->SetSize(baseSize_);
    hpBar_->SetSize(barSize_ );
}

void HPUISprite::Update(float _HP)
{
    _HP = std::clamp(_HP, 0.0f, 1.0f);
#ifdef _DEBUG
    if (ImGui::BeginTabItem(name_.c_str()))
    {
        ImGui::DragFloat2("basePos", &basePos_.x, 1.0f);
        ImGui::DragFloat2("barOffset", &barOffset_.x, 1.0f);
        ImGui::DragFloat2("baseSize", &baseSize_.x, 1.0f);
        ImGui::EndTabItem();
    }
#endif // _DEBUG



    frame_->translate_ = basePos_;
    base_ ->translate_ = basePos_;
    hpBar_->translate_ = basePos_ + barOffset_;

    frame_->SetSize(baseSize_);

    base_->SetSize(baseSize_);


    barSize_ = baseSize_ * _HP;
    barSize_.y = baseSize_.y;
    hpBar_->SetSize(barSize_);
}


void HPUISprite::Draw()
{
    base_->Draw();
    hpBar_->Draw();
    frame_->Draw();
}
