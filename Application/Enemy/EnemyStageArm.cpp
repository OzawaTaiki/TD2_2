#include "EnemyStageArm.h"
#include "VectorFunction.h"

void EnemyStageArm::Initialize(const Vector3& position, const Vector3& Velocity, Model* model)
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	model_ = model;

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Velocity;

	// Y軸周り角度(θy)
	worldTransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	worldTransform_.rotate_.x = std::atan2(velocity_.y, -length);
}

void EnemyStageArm::Update()
{
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 座標を移動させる
	if (deathTimer_ <= kLifeTime / 2) {
		worldTransform_.transform_ = worldTransform_.transform_ - velocity_;
	}
	else {
		worldTransform_.transform_ = worldTransform_.transform_ + velocity_;
	}

	worldTransform_.UpdateData();
}

void EnemyStageArm::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);
}

void EnemyStageArm::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
