
#include "EnemyStageArm.h"
#include "VectorFunction.h"
#include "Enemy.h"

void EnemyStageArm::Initialize(const Vector3& position, const Vector3& Velocity, Model* model, EnemyStageArm::AttackArm* attack)
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = position;
	worldTransform_.scale_ = { 4,4,4 };

	model_ = model;

	attack_ = attack;
	worldTransform2_.Initialize();
	worldTransform2_.transform_ = position;
	worldTransform2_.scale_ = { 8, 8, 8 };

	model2_ = Model::CreateFromObj("PredictionBox/PredictionBox.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });
	color2_.Initialize();
	color2_.SetColor(Vector4{ 1, 0, 0, 1 });

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Velocity;

	// Y軸周り角度(θy)
	worldTransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	worldTransform_.rotate_.x = std::atan2(velocity_.y, -length);

	worldTransform2_.rotate_ = worldTransform_.rotate_;

    collider_ = std::make_unique<Collider>();
    collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	collider_->SetShape(model_->GetMin(), model_->GetMax());
    collider_->SetAtrribute("enemyStageArm");
	collider_->SetMask({ "enemy" ,"enemyBullet"});
    collider_->SetGetWorldMatrixFunc([this]() {return worldTransform_.matWorld_; });
    collider_->SetOnCollisionFunc([this](const Collider* collider) {OnCollision(collider); });



}

void EnemyStageArm::Update()
{
	// 時間経過でデス
	// 時間経過でデス


	attackPreparationTime++;
	if (attackPreparationTime > attack_->MaxAttackPreparationTime) {

        collider_->RegsterCollider();

		if (length <= attack_->MaxLength) {
			worldTransform_.transform_ = worldTransform_.transform_ + velocity_;
			length += std::abs(velocity_.x);
			length += std::abs(velocity_.y);
			length += std::abs(velocity_.z);
		}
		else {
			armRetractTime++;
		}

		// 引っ込むまでの時間が過ぎたら、引っ込ませる
		if (armRetractTime >= attack_->MaxArmRetractTime) {
			worldTransform_.transform_ = worldTransform_.transform_ - velocity_;

			if (--deathTimer_ <= 0) {
				isDead_ = true;
			}
		}
	}




	worldTransform_.UpdateData();
	worldTransform2_.UpdateData();
}

void EnemyStageArm::Draw(const Camera& camera)
{
	if (attackPreparationTime < attack_->MaxAttackPreparationTime) {
		if (attack_->Maxblinking + attackPreparationTime > attack_->MaxAttackPreparationTime) {
			if (int(attackPreparationTime) % 5 == 0) {
				model2_->Draw(worldTransform2_, &camera, &color2_);
			}
		}
		else{
			model2_->Draw(worldTransform2_, &camera, &color2_);
		}
	}
	else {
		model_->Draw(worldTransform_, &camera, &color_);
	}

#ifdef _DEBUG
    collider_->Draw();
#endif // _DEBUG

}

void EnemyStageArm::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform2_.parent_ = parent;
}

void EnemyStageArm::OnCollision(const Collider* collider)
{
}
