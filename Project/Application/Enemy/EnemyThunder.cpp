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
	worldTransform2_.scale_ = { 1, 3, 1 };

	model2_ = Model::CreateFromObj("thunder/thunder.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 0, 0, 1 });
	color2_.Initialize();
	color2_.SetColor(Vector4{ 1, 1, 1, 1.0f });

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = Velocity;

	// Y軸周り角度(θy)
	worldTransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));

	// X軸周り角度(θx)
	worldTransform_.rotate_.x = std::atan2(velocity_.y, -length);
	worldTransform_.rotate_.x = 0;

	worldTransform2_.rotate_ = worldTransform_.rotate_;

    collider_ = std::make_unique<Collider>();
    collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	collider_->SetShape(model2_->GetMin(), model2_->GetMax());
    collider_->SetAtrribute("enemyThunder");
	collider_->SetMask({ "enemyThunder","enemy","enemyStageArm" });
    collider_->SetGetWorldMatrixFunc([this]() {return worldTransform2_.matWorld_; });
    collider_->SetOnCollisionFunc([this](const Collider* _collider) {isDead_ = true; });

}

void EnemyThunder::Update(Audio* audio, Audio::Sound sound)
{

	if (++attackPreparationTime_ > attack_->MaxAttackPreparationTime) {
		//if (++thicknessStartTime > attack_->MaxThicknessStartTime) {
			//if (++thunderStrikeTime > attack_->MaxThunderStrikeTime) {
		collider_->RegsterCollider();
		if (attack_->transitionFactor >= 0.9f) {
			if (attack_->transitionFactor <= 1.0f) {
				audio->SoundPlay(sound.soundDataHandle, sound.volume, 0, 0);
			}
			expandTime_++;
			worldTransform2_.scale_.x = MinMaxSize(attack_->MaxExpandTime, expandTime_, worldTransform2_.scale_.x);
			worldTransform2_.scale_.z = MinMaxSize(attack_->MaxExpandTime, expandTime_, worldTransform2_.scale_.z);
			//}
		}

		//}
			// 時間経過でデス
		if (worldTransform2_.scale_.z >= attack_->maxSize) {
			if (++deathTimer_ >= attack_->MaxDeathTimer_) {
				isDead_ = true;
			}
		}
	}

	worldTransform_.transform_ = worldTransform_.transform_;


	worldTransform_.UpdateData();
	worldTransform2_.UpdateData();
}

void EnemyThunder::Draw(const Camera& camera)
{
	if (attack_->clock == -1) {
		model2_->Draw(worldTransform2_, &camera, &color2_);
	}
	model_->Draw(worldTransform_, &camera, &color_);

#ifdef _DEBUG
	collider_->Draw();
#endif // DEBUG

}

void EnemyThunder::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	worldTransform2_.parent_ = parent;
}
