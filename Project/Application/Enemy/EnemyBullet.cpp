#include "EnemyBullet.h"
#include "VectorFunction.h"

void EnemyBullet::Initialize(const Vector3& position, const Vector3& Velocity, Model* model, EnemyBullet::Bullet* bullet)
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = position;

	model_ = model;

	bullet_ = bullet;

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

void EnemyBullet::Update()
{
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 座標を移動させる
	worldTransform_.transform_ = worldTransform_.transform_ + velocity_;

	worldTransform_.UpdateData();
}

void EnemyBullet::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);
}

void EnemyBullet::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

Vector3 StartEnd(Vector3& start, Vector3& end, float& t)
{
	if (t <= 0) {
		t = 0;
	}
	else if (t >= 1) {
		t = 1;
	}

	return Vector3(start.x + t * (end.x - start.x), start.y + t * (end.y - start.y), start.z + t * (end.z - start.z));
}
float StartEnd(float& start, float& end, float& t)
{
	if (t <= 0) {
		t = 0;
	}
	else if (t >= 1) {
		t = 1;
	}

	return (start + t * (end - start));
}
