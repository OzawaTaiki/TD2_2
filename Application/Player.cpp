#include "Player.h"

void Player::Initialize()
{

	worldTransform.Initialize();
	worldTransform.transform_ = Vector3{ 0,0,0 };

	//model_ = Model::CreateFromObj("Box/Box.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void Player::Update()
{

	worldTransform.UpdateData();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform, &camera, &color_);
}