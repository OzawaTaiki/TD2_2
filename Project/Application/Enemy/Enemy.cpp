#include <MatrixFunction.h>
#include "VectorFunction.h"

#include "Enemy.h"
#include "../Player.h"
#include "../../../Application/Stage/Stage.h"

#include "imgui.h"

template<typename T>
T Lerp(const T& a, const T& b, float t) {
	return a * (1.0f - t) + b * t;
}



void Enemy::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	model_ = Model::CreateFromObj("boss/boss.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

	modelBullet_ = Model::CreateFromObj("electricBall/electricBall.obj");;

	attackCamera_.Initialize();
	attackCamera_.translate_ = { 0,40,-80 };
	attackCamera_.rotate_ = { 0.55f,0,0 };

	attackCamera2_.Initialize();
	attackCamera2_.translate_ = { 0,35,-120 };
	attackCamera2_.rotate_ = { 0.25f,0,0 };


	ConfigManager::GetInstance()->LoadData();

	ConfigManager::GetInstance()->SetVariable("attackCamera1", "translate", &attackCamera2_.translate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera1", "rotate", &attackCamera2_.rotate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera3", "translate", &attackCamera_.translate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera3", "rotate", &attackCamera_.rotate_);

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
		case Behavior::kAttack2:
			BehaviorAttack2Initialize();
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
	case Behavior::kAttack2:
		BehaviorAttack2Update(); // 攻撃行動更新
		break;
	}

	//
	BulletUpdate();

	//
	StageArmUpdate();


	// HP
	if (hp <= 0) {
		isAlive = false;
	}

	if (ImGui::BeginTabBar("GameScene"))
	{
		if (ImGui::BeginTabItem("enemy"))
		{
			ImGui::DragFloat3("AttackCamera translate", &attackCamera_.translate_.x, 0.01f);
			ImGui::DragFloat3("AttackCamera rotate", &attackCamera_.rotate_.x, 0.01f);
			ImGui::DragFloat3("AttackCamera2 translate", &attackCamera2_.translate_.x, 0.01f);
			ImGui::DragFloat3("AttackCamera2 rotate", &attackCamera2_.rotate_.x, 0.01f);
			if (ImGui::Button("attack3")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
			}
			if (ImGui::Button("attack2")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack2;
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
	


	// ワールドトランスフォーム更新
	worldTransform_.UpdateData();

	attackCamera_.UpdateMatrix();
	attackCamera2_.UpdateMatrix();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);

	for (const auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

	for (const auto& bullet : stageArm) {
		bullet->Draw(camera);
	}

	switch (behavior_) {
	case Behavior::kRoot:
	case Behavior::kAttack2:
	default:

		break;
	case Behavior::kAttack:
		break;
	}
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

		float rotate = float(behaviorTimer_ / 3000);

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

void Enemy::StageArmInitialize(int num)
{
	const float kBulletSpeed = 1.4f;
	Vector3 velocityB{};

	// 
	Vector3 direction{};
	Vector3 stagePos{};
	direction = player_->GetWorldTransform().GetWorldPosition();

	// 床から
	if (num == Stage::StageNum::kFloor) {
		stagePos.x = player_->GetWorldTransform().GetWorldPosition().x;
		stagePos.y = stage_->GetWallFloor().y;
		stagePos.z = player_->GetWorldTransform().GetWorldPosition().z;
	}
	
	
	

	velocityB = Subtract(direction, stagePos);
	velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

	// 弾を生成し、初期化
	auto newBullet = std::make_unique<EnemyStageArm>();
	newBullet->Initialize(stagePos, velocityB, modelBullet_);

	// 弾の親設定
	newBullet->SetParent(worldTransform_.parent_);

	// 弾を登録する
	stageArm.push_back(std::move(newBullet));
}

void Enemy::StageArmUpdate()
{
	for (const auto& bullet : stageArm) {
		bullet->Update();
	}

	// デスフラグが立った弾を削除
	stageArm.remove_if([](const std::unique_ptr<EnemyStageArm>& bullet) { return bullet->IsDead(); });
}

void Enemy::BehaviorRootInitialize()
{
	// 浮遊ギミック
	InitializeFloatingGimmick();
}

void Enemy::BehaviorRootUpdate()
{
	// 回転と移動量の設定
	const float kMoveSpeed = 0.1f; // 移動速度
	// worldTransformBase_.rotation_.y += 0.00f; // 一定量のY軸回転

	// 向いている方向への移動ベクトルの計算
	Vector3 moveDirection = { 0.0f, 0.0f, kMoveSpeed };
	Matrix4x4 rotationMatrix = MakeRotateYMatrix(worldTransform_.rotate_.y);
	moveDirection = Transform(moveDirection, rotationMatrix);

	// ロックオン座標
	Vector3 lockOnPosition = player_->GetWorldTransform().GetWorldPosition();

	// 追跡対象からロックオン対象へのベクトル
	Vector3 sub = Subtract(-lockOnPosition, worldTransform_.transform_);

	// Y軸周り角度
	worldTransform_.rotate_.y = std::atan2(sub.x, sub.z);

	//worldTransform_.rotate_.y = -worldTransform_.rotate_.y;

	behaviorTimer_++;
	if (behaviorTimer_ >= 180) {
		behaviorTimer_ = 0;
		//behaviorRequest_ = Behavior::kAttack2;
	}

	//
	oldPos_ = worldTransform_.transform_;

	// 浮遊ギミック
	UpdateFloatingGimmick();
}

void Enemy::BehaviorAttackInitialize()
{
	attack3_.isBulletShot = false;
	attack3_.clock1 = 1;
	attack3_.transitionFactor = 0;
}

void Enemy::BehaviorAttackUpdate()
{

	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	Vector3 targetPos;
	if (attack3_.isBulletShot == false) {
		if (attack3_.clock1 == 1) {
			// 補間の進行度を更新
			if (attack3_.transitionFactor < 1.0f) {
				attack3_.transitionFactor += transitionSpeed;
			}
			else {
				attack3_.transitionFactor = 1.0f; // 補間が完了したら1.0fで固定
				attack3_.clock1 *= -1;
				attack3_.isBulletShot = false;
			}


			targetPos = attack3_.attackPos;

			worldTransform_.transform_ = Lerp(worldTransform_.transform_, targetPos, attack3_.transitionFactor);
		}
		else  if (attack3_.clock1 == -1) {
			// 補間の進行度を更新
			if (attack3_.transitionFactor > 0.0f) {
				attack3_.transitionFactor -= transitionSpeed * 3;
				if (attack3_.transitionFactor < 0.5f) {
					attack3_.isBulletShot = true;
				}
			}
			else {
				attack3_.transitionFactor = 0.0f; // 補間が完了したら0.0fで固定
				attack3_.clock1 *= -1;
			}

			targetPos.y = oldPos_.y;
			targetPos.x = oldPos_.x;
			targetPos.z = oldPos_.z;

			worldTransform_.transform_ = Lerp(targetPos, worldTransform_.transform_, attack3_.transitionFactor);
		}
	}


	if (attack3_.isBulletShot == true) {
		if (behaviorTimer_ <= 60) {
			if (behaviorTimer_ % 15 == 0 || behaviorTimer_ == 0) {
				BulletInitialize(worldTransform_.GetWorldPosition());
			}
		}

		behaviorTimer_++;
		if (behaviorTimer_ >= 120) {
			behaviorRequest_ = Behavior::kRoot;
			behaviorTimer_ = 0;
		}
	}
}

void Enemy::BehaviorAttack2Initialize()
{
	attack1_.isBulletShot = false;
	attack1_.clock1 = 1;
	attack1_.transitionFactor = 0;
}

void Enemy::BehaviorAttack2Update()
{
	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	Vector3 targetPos;
	if (attack1_.isBulletShot == false) {
		if (attack1_.clock1 == 1) {
			// 補間の進行度を更新
			if (attack1_.transitionFactor < 1.0f) {
				attack1_.transitionFactor += transitionSpeed;
			}
			else {
				attack1_.transitionFactor = 1.0f; // 補間が完了したら1.0fで固定
				attack1_.clock1 *= -1;
				attack1_.isBulletShot = true;
			}


			targetPos = attack1_.attackPos;

			worldTransform_.transform_ = Lerp(worldTransform_.transform_, targetPos, attack1_.transitionFactor);
		}
	}


	if (attack1_.isBulletShot == true) {
		if (behaviorTimer_ <= 60) {
			if (behaviorTimer_ % 15 == 0 || behaviorTimer_ == 0) {
				StageArmInitialize(Stage::kFloor);
			}
		}
		if (behaviorTimer_ >= 120) {
			if (attack1_.clock1 == -1) {
				// 補間の進行度を更新
				if (attack1_.transitionFactor > 0.0f) {
					attack1_.transitionFactor -= transitionSpeed * 3;
				}
				else {
					attack1_.transitionFactor = 0.0f; // 補間が完了したら0.0fで固定
					attack1_.clock1 *= -1;
				}

				targetPos.y = oldPos_.y;
				targetPos.x = oldPos_.x;
				targetPos.z = oldPos_.z;

				worldTransform_.transform_ = Lerp(targetPos, worldTransform_.transform_, attack1_.transitionFactor);
			}
		}

		behaviorTimer_++;
		if (behaviorTimer_ >= 600) {
			behaviorRequest_ = Behavior::kRoot;
			behaviorTimer_ = 0;
		}
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