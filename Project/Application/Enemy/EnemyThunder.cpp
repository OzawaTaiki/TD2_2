#include "EnemyThunder.h"
#include "VectorFunction.h"


float EnemyThunder::MinMaxSize(float& maxTime, float& time, float& size) {
	if (time <= 0) {
		return attack_->minSize;
	}
	else if (time >= maxTime) {
		return attack_->maxSize;
	}
	else { // 線形補間を用いてサイズを計算 
		float normalizedTime = time / maxTime;
		return attack_->minSize + (attack_->maxSize - attack_->minSize) * normalizedTime;
	}
};

void EnemyThunder::Initialize(const Vector3& position, const Vector3& Velocity, Model* model, EnemyThunder::AttackThunder* attack)
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = position;
	worldTransform_.scale_ = { 5,5,5 };

	model_ = model;

	attack_ = attack;

	worldTransform2_.Initialize();
	worldTransform2_.transform_ = position;
	worldTransform2_.scale_ = { 1, 5, 1 };

	model2_ = Model::CreateFromObj("PredictionBox/PredictionCylinder.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });
	color2_.Initialize();
	color2_.SetColor(Vector4{ 1, 0, 0, 0.6f });

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Velocity;

	// Y軸周り角度(θy)
	worldTransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	worldTransform_.rotate_.x = std::atan2(velocity_.y, -length);
	worldTransform_.rotate_.x = 0;

	worldTransform2_.rotate_ = worldTransform_.rotate_;


}

void EnemyThunder::Update()
{
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	attackPreparationTime_++;
	if (attackPreparationTime_ > attack_->MaxAttackPreparationTime) {
		if (++thicknessStartTime > attack_->MaxThicknessStartTime) {
			if (++thunderStrikeTime > attack_->MaxThunderStrikeTime) {
				expandTime_++;
				worldTransform2_.scale_.x = MinMaxSize(attack_->MaxExpandTime, expandTime_, worldTransform2_.scale_.x);
				worldTransform2_.scale_.z = MinMaxSize(attack_->MaxExpandTime, expandTime_, worldTransform2_.scale_.z);
			}
		}
	}



	worldTransform_.transform_ = worldTransform_.transform_;


	worldTransform_.UpdateData();
	worldTransform2_.UpdateData();
}

void EnemyThunder::Draw(const Camera& camera)
{
	if (thicknessStartTime > attack_->MaxThicknessStartTime) {
		model2_->Draw(worldTransform2_, &camera, &color2_);
	}
	model_->Draw(worldTransform_, &camera, &color_);
}

void EnemyThunder::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform2_.parent_ = parent;
}
