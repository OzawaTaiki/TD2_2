#include "ResultUI.h"
#include "ModelManager.h"
#include "Input.h"
void ResultUI::Initialize(const std::string& _scene)
{

    std::string label1;
    std::string label2;

    if (_scene=="Clear")
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


    titleButton_ = std::make_unique<UIButton>();
    titleButton_->Initialize(label2);

}

void ResultUI::Update()
{
    retryButton_->Update();
    titleButton_->Update();

    if (Input::GetInstance()->IsControllerConnected())
    {
        if (Input::GetInstance()->IsPadPressed(PadButton::iPad_Left) ||
            Input::GetInstance()->IsPadPressed(PadButton::iPad_Right))
        {
            selectButton_ = (selectButton_ + 1) % buttunNum_;
        }
    }

    if (Input::GetInstance()->IsKeyPressed(DIK_LEFTARROW) ||
        Input::GetInstance()->IsKeyPressed(DIK_RIGHTARROW))
    {
        selectButton_ = (selectButton_ + 1) % buttunNum_;
    }
}

void ResultUI::Draw()
{
    retryButton_->Draw();
    titleButton_->Draw();
}

#ifdef _DEBUG
#include <imgui.h>
void ResultUI::ImGui()
{

}
#endif // _DEBUG
