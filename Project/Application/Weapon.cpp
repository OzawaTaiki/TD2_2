#include "Weapon.h"
#include "../Collider/CollisionManager.h"

void Weapon::Initialize()
{
	worldTransform_.Initialize();
	model_ = Model::CreateFromObj("Sword/Sword.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

    collider_ = std::make_unique<Collider>();
	collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	collider_->SetShape(model_->GetMin(), model_->GetMax());
    collider_->SetAtrribute("weapon");
	collider_->SetMask({ "player","weapon" });
    collider_->SetGetWorldMatrixFunc([this]() { return worldTransform_.matWorld_; });
    collider_->SetOnCollisionFunc([this]() { OnCollision(); });

}

void Weapon::UpdateWorldTransform()
{
	worldTransform_.UpdateData();
}

void Weapon::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);

    collider_->Draw();
}

void Weapon::RegisterCollider()
{
	CollisionManager::GetInstance()->RegisterCollider(collider_.get());
}

void Weapon::OnCollision()
{
}
