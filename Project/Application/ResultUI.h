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

    uint32_t GetSelectButton() const { return selectButton_; }

private:

    std::unique_ptr<UIButton> retryButton_ = nullptr;
    std::unique_ptr<UIButton> titleButton_ = nullptr;

    uint32_t buttunNum_ = 2;
    uint32_t selectButton_ = 0;

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG

};