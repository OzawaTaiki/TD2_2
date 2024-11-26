#include "UIButton.h"

#include "ConfigManager.h"
#include "Input.h"

void UIButton::Initialize(const std::string& _label)
{
    UIBase::Initialize(_label);
}

void UIButton::Update()
{
}

void UIButton::Draw()
{
}

bool UIButton::IsClicked() const
{
    if (IsMousePointerInside() &&
        (Input::GetInstance()->IsMouseTriggered(0)||Input::GetInstance()->IsPadTriggered(PadButton::iPad_A)))
    {
        return true;
    }

    return false;
}
