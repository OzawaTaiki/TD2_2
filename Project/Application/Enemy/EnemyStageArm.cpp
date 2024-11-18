
#include "EnemyStageArm.h"
#include "VectorFunction.h"
#include "Enemy.h"

void EnemyStageArm::Initialize(const Vector3& position, const Vector3& Velocity, Model* model, EnemyStageArm::AttackArm* attack)
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = position;
	worldTransform_.scale_ = { 5,5,5 };

	model_ = model;

	attack_ = attack;
	worldTransform2_.Initialize();
	worldTransform2_.transform_ = position;
	worldTransform2_.scale_ = {5, 5, 5};

	model2_ = Model::CreateFromObj("PredictionBox/PredictionBox.obj");

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

	worldTransform2_.rotate_ = worldTransform_.rotate_;


}

void EnemyStageArm::Update()
{
	// 時間経過でデス
	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	attackPreparationTime++;
	if (attackPreparationTime > attack_->MaxAttackPreparationTime) {
		attaskMoveTime++;
		// 移動時間まで前に出る
		if (attaskMoveTime < attack_->MaxAttaskMoveTime) {
			worldTransform_.transform_ = worldTransform_.transform_ + velocity_;
			
		}// 時間が過ぎたら
		else {
			// 引っ込むまでの時間＋＋
			armRetractTime++;
		}

		// 引っ込むまでの時間が過ぎたら、引っ込ませる
		if (armRetractTime >= attack_->MaxArmRetractTime) {
			worldTransform_.transform_ = worldTransform_.transform_ - velocity_;
		}


	}
	
	
	

	worldTransform_.UpdateData();
	worldTransform2_.UpdateData();
}

void EnemyStageArm::Draw(const Camera& camera)
{
	if (attackPreparationTime < attack_->MaxAttackPreparationTime) {
		model2_->Draw(worldTransform2_, &camera, &color2_);
	}
	else {
		model_->Draw(worldTransform_, &camera, &color_);
	}
}

void EnemyStageArm::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform2_.parent_ = parent;
}
