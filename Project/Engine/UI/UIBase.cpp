#include "UIBase.h"

#include "Input.h"
#include "ConfigManager.h"
#include "TextureManager.h"

void UIBase::Initialize(const std::string& _label)
{
    label_ = _label;

    ConfigManager* configManager = ConfigManager::GetInstance();

    configManager->SetVariable(label_, "pos", &position_);
    configManager->SetVariable(label_, "size", &size_);
    configManager->SetVariable(label_, "anchor", &anchor_);
    configManager->SetVariable(label_, "isActive", reinterpret_cast<uint32_t*>(&isActive_));
    configManager->SetVariable(label_, "isVisible", reinterpret_cast<uint32_t*>(&isVisible_));
    configManager->SetVariable(label_, "textureName", reinterpret_cast<std::string*>(&textureName_));
    configManager->SetVariable(label_, "label", reinterpret_cast<std::string*>(&label_));
}

bool UIBase::IsMousePointerInside() const
{
    Vector2 mPos = Input::GetInstance()->GetMousePosition();

    return IsPointInside(mPos);
}

bool UIBase::IsPointInside(const Vector2& _point) const
{
    // アンカーを考慮した四頂点の計算
    Vector2 leftTop = position_ - size_ * anchor_;
    Vector2 rightBottom = position_ + size_ * (Vector2{ 1,1 } - anchor_);
    Vector2 rightTop = { rightBottom.x, leftTop.y };
    Vector2 leftBottom = { leftTop.x, rightBottom.y };

    // 四角形の内側にいるかどうか
    if (_point.x > leftTop.x && _point.x < rightBottom.x &&
        _point.y > leftTop.y && _point.y < rightBottom.y)
    {
        return true;
    }

    return false;
}

void UIBase::SetTextureNameAndLoad(const std::string& _textureName)
{
    textureName_ = _textureName;
    textureHandle_ = TextureManager::GetInstance()->Load(textureName_);
}

#ifdef _DEBUG
#include <ImGuiManager.h>
void UIBase::ImGui()
{
    ImGui::DragFloat2("position", &position_.x, 1.0f);
    ImGui::DragFloat2("size", &size_.x, 1.0f);
    ImGui::DragFloat2("anchor", &anchor_.x, 0.01f);
    ImGui::Checkbox("isActive", &isActive_);
    ImGui::Checkbox("isVisible", &isVisible_);
    ImGui::InputText("label", reinterpret_cast<char*>(&label_),255);
    ImGui::InputText("textureName", reinterpret_cast<char*>(&textureName_), 255);
    if (ImGui::Button("save"))
    {
        ConfigManager::GetInstance()->SaveData(label_);
    }

}
#endif // _DEBUG
