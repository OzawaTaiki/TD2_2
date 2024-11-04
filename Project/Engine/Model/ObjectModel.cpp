#include "ObjectModel.h"
#include "DXCommon.h"
#include "ModelManager.h"

void ObjectModel::Initialize(const std::string& _filePath)
{
    model_ = Model::CreateFromObj(_filePath);

    worldTransform_.Initialize();
    objectColor_ = std::make_unique<ObjectColor>();
    objectColor_->Initialize();

}

void ObjectModel::Update()
{
    model_->Update();
    worldTransform_.UpdateData({ model_->GetNodeMatrix(),model_->GetAnimationMatrix()});
    worldTransform_.GetWorldPosition().ShowData("w", false);
}

void ObjectModel::Draw(const Camera* _camera, const Vector4& _color)
{
    objectColor_->SetColor(_color);

    ModelManager::GetInstance()->PreDraw();

    auto commandList = DXCommon::GetInstance()->GetCommandList();
    _camera->QueueCommand(commandList, 0);
    worldTransform_.QueueCommand(commandList, 1);
    objectColor_->QueueCommand(commandList, 3);
    model_->QueueCommandAndDraw(commandList);// BVB IBV MTL2 TEX4 LIGHT567

}