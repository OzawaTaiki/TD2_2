#include "Weapon.h"

void Weapon::Initialize()
{
	worldTransform_.Initialize();
	model_ = Model::CreateFromObj("Sword/Sword.obj");
	
	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void Weapon::UpdateWorldTransform()
{
	worldTransform_.UpdateData();
}

void Weapon::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);
}
