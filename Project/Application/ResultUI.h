#pragma once

#include "Camera.h"
#include "UIButton.h"

#include <memory>

class ResultUI
{
public:

    ResultUI() = default;
    ~ResultUI() = default;

    void Initialize(const std::string& _scene);
    void Update();
    void Draw();

    void DrawBackGround();

    uint32_t GetSelectButton() const { return selectButton_; }

private:

    void ButtonUpdate();

    std::string scene_;

    std::unique_ptr<UIButton> retryButton_ = nullptr;
    std::unique_ptr<UIButton> titleButton_ = nullptr;

    Sprite* backGround_ = nullptr;
    std::string backGroundTexturePath_ = "white.png";

    Sprite* text_ = nullptr;
    std::string textTexturePath_ = "white.png";
    Vector2 textSize_ = {};
    Vector2 textBasePos_ = {};
    float textfloatingSpeed_ = 1.0f;
    float textfloatingRange_ = 1.0f;
    float currentTextTimer_ = 0.0f;


    std::string retryButtonTexturePath_ = "white.png";
    std::string titleButtonTexturePath_ = "white.png";

    uint32_t buttunNum_ = 2;
    int32_t selectButton_ = 0;

    Vector2 baseSize_ = {};

    Vector2 retrySize_ = {};
    Vector2 titleSize_ = {};

    Vector2 sizeRange_ = {};
    float resizeSpeed_ = 1.0f;

    float currentRetryTimer_ = 0.0f;
    float currentTitleTimer_ = 0.0f;

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG

};