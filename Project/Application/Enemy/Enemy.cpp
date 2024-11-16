#include <MatrixFunction.h>
#include "VectorFunction.h"
#include "../Collider/CollisionManager.h"

#include "Enemy.h"
#include "../Player.h"




void Enemy::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	model_ = Model::CreateFromObj("Arrow/Arrow.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

	modelBullet_ = Model::CreateFromObj("Arrow/Arrow.obj");

    collider_ = std::make_unique<Collider>();
	collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
    collider_->SetShape(model_->GetMin(), model_->GetMax());
    collider_->SetAtrribute("enemy");
    collider_->SetMask("enemy");
    collider_->SetGetWorldMatrixFunc([this]() { return worldTransform_.matWorld_; });
    collider_->SetOnCollisionFunc([this]() { OnCollision(); });



}

void Enemy::Update()
{
	if (behaviorRequest_) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			//BehaviorJumpInitialize();
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Behavior::kRoot: // 通常行動更新
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack: // 攻撃行動更新
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		//BehaviorJumpUpdate(); // ジャンプ行動更新
		break;
	}

	BulletUpdate();
	// HP
	if (hp <= 0) {
		isAlive = false;
	}

    CollisionManager::GetInstance()->RegisterCollider(collider_.get());

	// ワールドトランスフォーム更新
	worldTransform_.UpdateData();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);

	for (const auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

	switch (behavior_) {
	case Behavior::kRoot:
	case Behavior::kJump:
	default:

		break;
	case Behavior::kAttack:
		break;
	}
	collider_->Draw();
}

void Enemy::BulletInitialize(Vector3 pos)
{
	const float kBulletSpeed = 0.4f;
	Vector3 velocityB{};

	const int numBullets = 16;
	const float angleStep = 360.0f / numBullets;

	for (int i = 0; i < numBullets; i++) {
		float angle = i * angleStep;
		float radian = angle * (3.14f / 180.0f);  // Convert to radians

		float rotate = static_cast<float> (behaviorTimer_) / 3000.0f;

		Vector3 direction{ cos(radian + behaviorTimer_), pos.y, sin(radian + behaviorTimer_) };

		velocityB = Subtract(direction, pos);
		velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

		// 弾を生成し、初期化
		auto newBullet = std::make_unique<EnemyBullet>();
		newBullet->Initialize(pos, velocityB, modelBullet_);

		// 弾の親設定
		newBullet->SetParent(worldTransform_.parent_);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}

}

void Enemy::BulletUpdate()
{
	for (const auto& bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグが立った弾を削除
	bullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });
}

void Enemy::OnCollision()
{
}

void Enemy::BehaviorRootInitialize()
{
	// 浮遊ギミック
	InitializeFloatingGimmick();
}

void Enemy::BehaviorRootUpdate()
{
	velocity_ = { 0.0f, 0.0f, 0.0f };
	speed = 0.2f;

	behaviorTimer_++;
	if (behaviorTimer_ >= 180) {
		behaviorTimer_ = 0;
		behaviorRequest_ = Behavior::kAttack;
	}

	// 浮遊ギミック
	UpdateFloatingGimmick();
}

void Enemy::BehaviorAttackInitialize()
{

}

void Enemy::BehaviorAttackUpdate()
{
	behaviorTimer_++;
	if (behaviorTimer_ >= 120) {
		behaviorTimer_ = 0;
		behaviorRequest_ = Behavior::kRoot;
	}

	if (behaviorTimer_ % 15 == 0) {
		BulletInitialize(worldTransform_.GetWorldPosition());
	}
}

void Enemy::InitializeFloatingGimmick()
{
	floatingParameter_ = 0.0f;
}

void Enemy::UpdateFloatingGimmick()
{
	float pi = 3.141592f;
	// 1フレームでのパラメータ加算値
	const float step = 2.0f * pi / floatingPeriod;
	// パラメータを1ステップ分加算
	floatingParameter_ += step;

	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * pi);

	float floatingAmplitude = 0.5f;

	// 浮遊を座標に反映
	worldTransform_.transform_.y = std::sin(floatingParameter_) * floatingAmplitude;
}
