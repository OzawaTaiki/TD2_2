#include "ObjectModel.h"
#include "DXCommon.h"
#include "ModelManager.h"
#include "MatrixFunction.h"
#include "../Collider/CollisionManager.h"

void ObjectModel::Initialize(const std::string& _filePath)
{
    model_ = Model::CreateFromObj(_filePath);

    worldTransform_.Initialize();
    objectColor_ = std::make_unique<ObjectColor>();
    objectColor_->Initialize();


}

void ObjectModel::Update()
{
#ifdef _DEBUG
    ImGui();
#endif // _DEBUG
    worldTransform_.UpdateData();
}

void ObjectModel::Draw(const Camera* _camera, const Vector4& _color)
{
    objectColor_->SetColor(_color);

    ModelManager::GetInstance()->PreDrawForObjectModel();

    auto commandList = DXCommon::GetInstance()->GetCommandList();
    _camera->QueueCommand(commandList, 0);
    worldTransform_.QueueCommand(commandList, 1);
    objectColor_->QueueCommand(commandList, 3);
    model_->QueueCommandAndDraw(commandList);// BVB IBV MTL2 TEX4 LIGHT567

    collider_->Draw();

    //model_->DrawSkeleton(worldTransform_.matWorld_);
}

#ifdef _DEBUG
#include <imgui.h>
void ObjectModel::ImGui()
{
    ImGui::PushID(this);
    ImGui::DragFloat3("Translate", &worldTransform_. transform_.x, 0.01f);
    ImGui::DragFloat3("Scale", &worldTransform_.scale_.x, 0.01f);
    ImGui::DragFloat3("Rotate", &worldTransform_.rotate_.x, 0.01f);
    if(ImGui::DragFloat3("ReferencePoint", &refPoint.x, 0.01f))
        collider_->SetReferencePoint(refPoint);
    ImGui::PopID();
}
#endif // _DEBUG
