#include <MatrixFunction.h>
#include "VectorFunction.h"
#include "../Collider/CollisionManager.h"

#include "Enemy.h"
#include "../Player.h"
#include "../../../Application/Stage/Stage.h"

#include "imgui.h"

template<typename T>
T Lerp(const T& a, const T& b, float t) {
	return a * (1.0f - t) + b * t;
}

float easyInOutElastic(float t) {
	const float c5 = (2 * M_PI) / 4.5;
	if (t == 0) {
		return 0.0f;
	}
	else if (t == 1) {
		return 1.0f;
	}
	if (t < 0.5) {
		return -(pow(2, 20 * t - 10) * sin((20 * t - 11.125) * c5)) / 2.0f;
	}
	else {
		return (pow(2, -20 * t + 10) * sin((20 * t - 11.125) * c5)) / 2.0f + 1.0f;
	}
}

void Enemy::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;


	worldTransformLeft_.Initialize();
	worldTransformLeft_.parent_ = &worldTransformBody_;
	worldTransformLeft_.transform_ = Vector3{ 0,0,0 };

	worldTransformRight_.Initialize();
	worldTransformRight_.parent_ = &worldTransformBody_;
	worldTransformRight_.transform_ = Vector3{ 0,0,0 };


	model_ = Model::CreateFromObj("boss/boss.obj");
	modelLeftArm_ = Model::CreateFromObj("bossAtackArm/bossAtackArmLeft.obj");
	modelRightArm_ = Model::CreateFromObj("bossAtackArm/bossAtackArmRight.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

	modelBullet_ = Model::CreateFromObj("electricBall/electricBall.obj");;
	modelStageArm_ = Model::CreateFromObj("bossAtackArm/bossAtackArm.obj");;
	modelThunder_ = Model::CreateFromObj("PredictionBox/PredictionSphere.obj");;


	attackCamera_.Initialize();
	attackCamera_.translate_ = { 0,35,-120 };
	attackCamera_.rotate_ = { 0.25f,0,0 };

	attackCamera2_.Initialize();
	attackCamera2_.translate_ = { 0,35,-120 };
	attackCamera2_.rotate_ = { 0.25f,0,0 };

	attackCamera3_.Initialize();
	attackCamera3_.translate_ = { 0,250,-200 };
	attackCamera3_.rotate_ = { 0.55f,0,0 };





	ConfigManager::GetInstance()->SetVariable("attackCamera1", "translate", &attackCamera_.translate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera1", "rotate", &attackCamera_.rotate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera2", "translate", &attackCamera2_.translate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera2", "rotate", &attackCamera2_.rotate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera3", "translate", &attackCamera3_.translate_);
	ConfigManager::GetInstance()->SetVariable("attackCamera3", "rotate", &attackCamera3_.rotate_);


	// 攻撃1
	ConfigManager::GetInstance()->SetVariable("attack1", "attackPos", &attack1_.attackPos);
	ConfigManager::GetInstance()->SetVariable("attack1", "attackPower", &attack1_.attackPower);
	ConfigManager::GetInstance()->SetVariable("attack1", "andingTime", &attack1_.MaxLandingTime);
	float armmax = float(attack1_.MaxArmNum);
	ConfigManager::GetInstance()->SetVariable("attack1", "armNum", &armmax);
	ConfigManager::GetInstance()->SetVariable("attack1", "moveTime", &attack1_.MaxAttaskMoveTime);
	ConfigManager::GetInstance()->SetVariable("attack1", "preparationTime", &attack1_.MaxAttackPreparationTime);
	ConfigManager::GetInstance()->SetVariable("attack1", "armSpeed", &attack1_.armSpeed);
	ConfigManager::GetInstance()->SetVariable("attack1", "ToNextPredictionDelay", &attack1_.MaxAttackToNextPredictionDelay);
	ConfigManager::GetInstance()->SetVariable("attack1", "weakArm", &attack1_.weakArmSpawnProbability);
	// 攻撃2
	ConfigManager::GetInstance()->SetVariable("attack2", "attackPreparationTime", &attack2_.MaxAttackPreparationTime);
	ConfigManager::GetInstance()->SetVariable("attack2", "attackPower", &attack2_.attackPower);
	ConfigManager::GetInstance()->SetVariable("attack2", "landingTime", &attack2_.MaxLandingTime);
	ConfigManager::GetInstance()->SetVariable("attack2", "expandTime", &attack2_.MaxExpandTime);
	ConfigManager::GetInstance()->SetVariable("attack2", "thicknessStartTime", &attack2_.MaxThicknessStartTime);
	ConfigManager::GetInstance()->SetVariable("attack2", "thunderStrikeTime", &attack2_.MaxThunderStrikeTime);
	ConfigManager::GetInstance()->SetVariable("attack2", "maxSize", &attack2_.maxSize);
	ConfigManager::GetInstance()->SetVariable("attack2", "minSize", &attack2_.minSize);

	// 攻撃3
	ConfigManager::GetInstance()->SetVariable("attack3", "cooldown", &attack3_.MaxAttackCooldown);
	ConfigManager::GetInstance()->SetVariable("attack3", "attackPower", &attack3_.attackPower);
	float ShotsPerPhase = int(attack3_.MaxNumShotsPerPhase);
	ConfigManager::GetInstance()->SetVariable("attack3", "numShotsPerPhase", &ShotsPerPhase);
	ConfigManager::GetInstance()->SetVariable("attack3", "speed", &attack3_.speed);
	
	// 攻撃4
	ConfigManager::GetInstance()->SetVariable("attack4", "MaxRotateSpeed", &attack4_.MaxRotateSpeed);
	ConfigManager::GetInstance()->SetVariable("attack4", "MinRotateSpeed", &attack4_.MinxRotateSpeed);
	ConfigManager::GetInstance()->SetVariable("attack4", "SpinTime", &attack4_.MaxSpinTime);
	ConfigManager::GetInstance()->SetVariable("attack4", "speed", &attack4_.speed);
	ConfigManager::GetInstance()->SetVariable("attack4", "ArmGrowthToSpinDelay", &attack4_.MaxArmGrowthToSpinDelay);
	ConfigManager::GetInstance()->SetVariable("attack4", "StoppingTime", &attack4_.MaxStoppingTime);
	ConfigManager::GetInstance()->SetVariable("attack4", "cooldownTime", &attack4_.cooldownTime);
	

}

void Enemy::Update()
{
	if (behaviorRequest_) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kFear:
			BehaviorFearInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kAttack2:
			BehaviorAttack2Initialize();
			break;
		case Behavior::kAttack3:
			BehaviorAttack3Initialize();
			break;
		case Behavior::kAttack4:
			BehaviorAttack4Initialize();
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Behavior::kRoot: // 通常行動更新
		BehaviorRootUpdate();
		break;
	case Behavior::kFear: // 怯み行動更新
		BehaviorFearUpdate();
		break;
	case Behavior::kAttack: // 攻撃行動更新
		BehaviorAttackUpdate();
		break;
	case Behavior::kAttack2:
		BehaviorAttack2Update(); // 攻撃行動更新
		break;
	case Behavior::kAttack3:
		BehaviorAttack3Update(); // 攻撃行動更新
		break;
	case Behavior::kAttack4:
		BehaviorAttack4Update(); // 攻撃行動更新
		break;
	}

	//
	BulletUpdate();

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
			ImGui::DragFloat3("AttackCamera3 translate", &attackCamera3_.translate_.x, 0.01f);
			ImGui::DragFloat3("AttackCamera3 rotate", &attackCamera3_.rotate_.x, 0.01f);
			if (ImGui::Button("attack")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
			}
			if (ImGui::Button("attack2")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack2;
			}
			if (ImGui::Button("attack3")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack3;
			}
			if (ImGui::Button("attack4")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack4;
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::BeginTabBar("Boss"))
	{
		if (ImGui::BeginTabItem("attack1"))
		{
			ImGui::DragFloat3("pos", &attack1_.attackPos.x, 0.01f);
			ImGui::DragFloat("armSpeed", &attack1_.armSpeed, 0.01f);
			ImGui::DragInt("armNum", &attack1_.MaxArmNum, 0.01f);
			ImGui::DragFloat("attackPower", &attack1_.attackPower, 0.01f);
			ImGui::DragFloat("poreparationTime", &attack1_.MaxAttackPreparationTime, 0.01f);
			ImGui::DragFloat("moveTime", &attack1_.MaxAttaskMoveTime, 0.01f);
			ImGui::DragFloat("armRetractTime", &attack1_.MaxArmRetractTime, 0.01f);
			ImGui::DragFloat("ToNextPredictionDelay", &attack1_.MaxAttackToNextPredictionDelay, 0.01f);
			ImGui::DragFloat("LandingTime", &attack1_.MaxLandingTime, 0.01f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("attack2"))
		{
			ImGui::DragFloat3("pos", &attack2_.attackPos.x, 0.01f);
			ImGui::DragFloat("ExpandTime", &attack2_.MaxExpandTime, 0.01f);
			ImGui::DragFloat("thicknessStartTime", &attack2_.MaxThicknessStartTime, 0.01f);
			ImGui::DragFloat("attackPower", &attack2_.attackPower, 0.01f);
			ImGui::DragFloat("poreparationTime", &attack2_.MaxAttackPreparationTime, 0.01f);
			ImGui::DragFloat("thunderStrikeTime", &attack2_.MaxThunderStrikeTime, 0.01f);
			ImGui::DragFloat("maxSize", &attack2_.maxSize, 0.01f);
			ImGui::DragFloat("minSize", &attack2_.minSize, 0.01f);
			ImGui::DragFloat("LandingTime", &attack2_.MaxLandingTime, 0.01f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("attack3"))
		{
			ImGui::DragFloat3("pos", &attack3_.attackPos.x, 0.01f);
			ImGui::DragFloat("cooldown", &attack3_.MaxAttackCooldown, 0.01f);
			ImGui::DragInt("numShotsPerPhase", &attack3_.MaxNumShotsPerPhase, 0.01f);
			ImGui::DragFloat("attackPower", &attack3_.attackPower, 0.01f);
			ImGui::DragFloat("speed", &attack3_.speed, 0.01f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("attack4"))
		{
			ImGui::DragFloat3("ArmGrowthToSpinDelay", &attack4_.MaxArmGrowthToSpinDelay, 0.01f);
			ImGui::DragFloat("cooldown", &attack4_.cooldownTime, 0.01f);
			ImGui::DragFloat("SpinTime", &attack4_.MaxSpinTime, 0.01f);
			ImGui::DragFloat("MaxRotateSpeed", &attack4_.MaxRotateSpeed, 0.01f);
			ImGui::DragFloat("MinRotateSpeed", &attack4_.MinxRotateSpeed, 0.01f);
			ImGui::DragFloat("StoppingTime", &attack4_.MaxStoppingTime, 0.01f);
			ImGui::DragFloat("speed", &attack4_.speed, 0.01f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}



	// ワールドトランスフォーム更新
	worldTransform_.UpdateData();
	worldTransformLeft_.UpdateData();
	worldTransformRight_.UpdateData();
	worldTransformBody_.UpdateData();

	attackCamera_.UpdateMatrix();
	attackCamera2_.UpdateMatrix();
	attackCamera3_.UpdateMatrix();
}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransformBody_, &camera, &color_);

	for (const auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

	for (const auto& bullet : stageArm) {
		bullet->Draw(camera);
	}

	for (const auto& bullet : thunder_) {
		bullet->Draw(camera);
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		break;
	case Behavior::kAttack:
		break;
	case Behavior::kAttack2:
		break;
	case Behavior::kAttack3:
		break;
	case Behavior::kAttack4:
		modelLeftArm_->Draw(worldTransformLeft_, &camera, &color_);
		modelRightArm_->Draw(worldTransformRight_, &camera, &color_);
		break;
	}
	collider_->Draw();
}



#pragma region Attack1

void Enemy::StageArmInitialize(int num)
{
	const float kBulletSpeed = attack1_.armSpeed;
	Vector3 velocityB{};

	// 
	Vector3 direction{};
	Vector3 stagePos{};
	direction = player_->GetWorldTransform().GetWorldPosition();

	// 床から
	if (num == Stage::StageNum::kFloor) {
		stagePos.x = player_->GetWorldTransform().GetWorldPosition().x;
		stagePos.y = stage_->GetWallFloor().y - 20;
		stagePos.z = player_->GetWorldTransform().GetWorldPosition().z;
	}
	else if (num == Stage::StageNum::kBack) {
		stagePos.x = player_->GetWorldTransform().GetWorldPosition().x;
		stagePos.y = player_->GetWorldTransform().GetWorldPosition().y;
		stagePos.z = stage_->GetWallBack().z + 20;
	}
	else if (num == Stage::StageNum::kLeft) {
		stagePos.x = stage_->GetWallLeft().x - 20;
		stagePos.y = player_->GetWorldTransform().GetWorldPosition().y;
		stagePos.z = player_->GetWorldTransform().GetWorldPosition().z;
	}
	else if (num == Stage::StageNum::kRight) {
		stagePos.x = stage_->GetWallRight().x + 20;
		stagePos.y = player_->GetWorldTransform().GetWorldPosition().y;
		stagePos.z = player_->GetWorldTransform().GetWorldPosition().z;
	}




	velocityB = Subtract(direction, stagePos);
	velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

	// 弾を生成し、初期化
	auto newBullet = std::make_unique<EnemyStageArm>();
	newBullet->Initialize(stagePos, velocityB, modelStageArm_, &attack1_);

	// 弾の親設定
	newBullet->SetParent(worldTransform_.parent_);

	// 弾を登録する
	stageArm.push_back(std::move(newBullet));
}

void Enemy::BehaviorAttackInitialize()
{
	attack1_.isBulletShot = false;
	attack1_.clock1 = 1;
	attack1_.transitionFactor = 0;

	attack1_.armNum = 0;
	attack1_.landingTime = 0;

	srand(unsigned int(time(nullptr))); // シードを現在の時刻で設定


}

void Enemy::BehaviorAttackUpdate()
{
	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）




	Vector3 targetPos;
	// 上に上がる処理
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


			targetPos.x = worldTransform_.transform_.x;
			targetPos.y = attack1_.attackPos.y;
			targetPos.z = worldTransform_.transform_.z;

			worldTransform_.transform_ = Lerp(worldTransform_.transform_, targetPos, attack1_.transitionFactor);
		}
	}

	// 上がった時の処理
	if (attack1_.isBulletShot == true) {
		if (++attack1_.AttackToNextPredictionDelay >= attack1_.MaxAttackToNextPredictionDelay)
			if (attack1_.armNum < attack1_.MaxArmNum) {


				int newLocation;
				do {
					newLocation = rand() % 4 + 1;
				} while (newLocation == attack1_.oldAttackSpawnLocation);

				StageArmInitialize(attack1_.attackSpawnLocation);

				attack1_.attackSpawnLocation = newLocation;
				attack1_.oldAttackSpawnLocation = newLocation;
				attack1_.AttackToNextPredictionDelay = 0;
				attack1_.armNum++;
			}



		if (attack1_.armNum >= attack1_.MaxArmNum) {
			if (++attack1_.landingTime >= attack1_.MaxLandingTime) {
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
				if (++behaviorTimer_ >= 120) {
					behaviorRequest_ = Behavior::kRoot;
					behaviorTimer_ = 0;
				}
			}
		}
	}
}

#pragma endregion // 攻撃1

#pragma region Attack2

void Enemy::ThunderInitialize(Vector3 pos)
{
	const float kBulletSpeed = 0.4f;
	Vector3 velocityB{};
	//Vector3 direction{ };



	for (int x = 0; x < 10; x++) {
		for (int z = 0; z < 10; z++) {
			Vector3 pospos{};
			Vector3 direction{};
			pospos.x = float(x * 20) - 90;
			pospos.y = stage_->GetWallFloor().y;
			pospos.z = float(z * 20) - 90;

			direction = pospos;
			direction.y = pospos.y + 2;

			velocityB = Subtract(direction, pospos);
			velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

			// 弾を生成し、初期化
			auto newBullet = std::make_unique<EnemyThunder>();
			newBullet->Initialize(pospos, velocityB, modelThunder_, &attack2_);

			// 弾の親設定
			newBullet->SetParent(worldTransform_.parent_);

			// 弾を登録する
			thunder_.push_back(std::move(newBullet));
		}
	}
}

void Enemy::BehaviorAttack2Initialize()
{
	attack2_.isBulletShot = false;
	attack2_.clock = 1;
	attack2_.transitionFactor = 0;

	attack2_.landingTime = 0;
}

void Enemy::BehaviorAttack2Update()
{
	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	Vector3 targetPos;
	if (attack2_.isBulletShot == false) {


		if (attack2_.clock == 1) {
			if (attack2_.transitionFactor == 0) {
				ThunderInitialize(0);
			}
			// 補間の進行度を更新
			if (attack2_.transitionFactor < 1.0f) {
				attack2_.transitionFactor += transitionSpeed;
			}
			else {
				attack2_.transitionFactor = 1.0f; // 補間が完了したら1.0fで固定
				attack2_.clock *= -1;
				attack2_.isBulletShot = false;
			}


			targetPos = attack2_.attackPos;

			worldTransform_.transform_ = Lerp(worldTransform_.transform_, targetPos, attack2_.transitionFactor);
		}
	}

	if (++attack2_.landingTime >= attack2_.MaxLandingTime) {
		// 補間の進行度を更新
		if (attack2_.transitionFactor > 0.0f) {
			attack2_.transitionFactor -= transitionSpeed * 3;
			if (attack2_.transitionFactor < 0.5f) {
				attack2_.isBulletShot = true;
			}
		}
		else {
			attack2_.transitionFactor = 0.0f; // 補間が完了したら0.0fで固定
			attack2_.clock *= -1;
		}

		targetPos.y = oldPos_.y;
		targetPos.x = oldPos_.x;
		targetPos.z = oldPos_.z;

		worldTransform_.transform_ = Lerp(targetPos, worldTransform_.transform_, attack2_.transitionFactor);
	}

	if (attack2_.isBulletShot == true) {

		behaviorTimer_++;
		if (behaviorTimer_ >= 180) {
			behaviorRequest_ = Behavior::kRoot;
			behaviorTimer_ = 0;
		}
	}
}

#pragma endregion // 攻撃2

#pragma region Attack3

void Enemy::BulletInitialize(Vector3 pos)
{
	const float kBulletSpeed = attack3_.speed;
	Vector3 velocityB{};

	const int numBullets = attack3_.numElectricCount;
	const float angleStep = 360.0f / numBullets;

	for (int i = 0; i < numBullets; i++) {
		float angle = i * angleStep;
		float radian = angle * (3.14f / 180.0f);  // Convert to radians

		float rotate = float(attack3_.numShotsPerPhase / 3000);

		Vector3 direction{ cosf(radian + attack3_.numShotsPerPhase) + pos.x, pos.y, sinf(radian + attack3_.numShotsPerPhase) + pos.z };

		velocityB = Subtract(direction, pos);
		velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

		// 弾を生成し、初期化
		auto newBullet = std::make_unique<EnemyBullet>();
		newBullet->Initialize(pos, velocityB, modelBullet_, &attack3_);

		// 弾の親設定
		newBullet->SetParent(worldTransform_.parent_);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Enemy::BulletUpdate()
{
	for (const auto& bullet : stageArm) {
		bullet->Update();
	}

	// デスフラグが立った弾を削除
	stageArm.remove_if([](const std::unique_ptr<EnemyStageArm>& bullet) { return bullet->IsDead(); });

	for (const auto& bullet : bullets_) {
		bullet->Update();
	}
	// デスフラグが立った弾を削除
	bullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });


	for (const auto& bullet : thunder_) {
		bullet->Update();
	}
	// デスフラグが立った弾を削除
	thunder_.remove_if([](const std::unique_ptr<EnemyThunder>& bullet) { return bullet->IsDead(); });
}

void Enemy::OnCollision()
{
}

void Enemy::BehaviorRootInitialize()
{
	attack3_.isBulletShot = false;
	attack3_.clock1 = 1;
	attack3_.transitionFactor = 0;

	attack3_.numShotsPerPhase = 0;
	attack3_.attackCooldown = attack3_.MaxAttackCooldown;
}

void Enemy::BehaviorAttack3Update()
{
	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）


	Vector3 targetPos;
	Vector3 topPos;
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

				attack3_.MaxPosY = worldTransform_.transform_.y;
			}
			targetPos.x = worldTransform_.transform_.x;
			targetPos.y = attack3_.attackPos.y;
			targetPos.z = worldTransform_.transform_.z;

			worldTransform_.transform_ = StartEnd(worldTransform_.transform_, targetPos, attack3_.transitionFactor);
		}
		else  if (attack3_.clock1 == -1) {
			// 補間の進行度を更新
			if (attack3_.transitionFactor > 0.0f) {
				attack3_.transitionFactor -= transitionSpeed;
				if (attack3_.transitionFactor < 0.5f) {
					attack3_.isBulletShot = true;
				}
			}
			else {
				attack3_.transitionFactor = 0.0f; // 補間が完了したら0.0fで固定
				attack3_.clock1 *= -1;
			}

			targetPos.y = oldPos_.y;
			targetPos.x = worldTransform_.transform_.x;
			targetPos.z = worldTransform_.transform_.z;

			topPos = targetPos;

			topPos.y = attack3_.MaxPosY;

			worldTransform_.transform_ = StartEnd(topPos, targetPos, attack3_.transitionFactor);
		}
	}







	if (attack3_.isBulletShot == true) {
		if (attack3_.numShotsPerPhase < attack3_.MaxNumShotsPerPhase) {
			if (++attack3_.attackCooldown >= attack3_.MaxAttackCooldown) {
				BulletInitialize(worldTransform_.GetWorldPosition());
				attack3_.numShotsPerPhase++;
				attack3_.attackCooldown = 0;
			}
		}

		if (attack3_.numShotsPerPhase >= attack3_.MaxNumShotsPerPhase) {
			behaviorTimer_++;
		}

		if (behaviorTimer_ >= 120) {
			behaviorRequest_ = Behavior::kRoot;
			behaviorTimer_ = 0;
		}
	}
}

#pragma endregion // 攻撃3

#pragma region Attack4

void Enemy::BehaviorAttack4Initialize()
{
	attack4_.transitionFactor = 0;
	worldTransformLeft_.transform_ = { 0,0,0 };
	worldTransformRight_.transform_ = { 0,0,0 };
	attack4_.spinTime = 0;
	attack4_.armGrowthToSpinDelay = 0;
	attack4_.stoppingTime = 0;
}

void Enemy::BehaviorAttack4Update() {
	static float transitionFactor = 0.01f;

	attack4_.transitionFactor += transitionFactor;

	float str = 0;
	float end = 4;
	float endm = -4;
	if (++attack4_.armGrowthToSpinDelay <= attack4_.MaxArmGrowthToSpinDelay) {
		worldTransformLeft_.transform_.x = StartEnd(str, endm, attack4_.transitionFactor);
		worldTransformRight_.transform_.x = StartEnd(str, end, attack4_.transitionFactor);

	}
	else {
		if (++attack4_.spinTime <= attack4_.MaxSpinTime) {
			Move(attack4_.speed, true);

			attack4_.rotateT += transitionFactor;
			if (attack4_.rotateT >= 1.0f) {
				attack4_.rotateT = 1;
			}
			attack4_.rotateSpeed = Lerp(attack4_.MinxRotateSpeed, attack4_.MaxRotateSpeed, attack4_.rotateT);

			worldTransformBody_.rotate_.y += attack4_.rotateSpeed;


		}
		else {
			attack4_.rotateT -= transitionFactor;
			if (attack4_.rotateT <= 0.0f) {
				attack4_.rotateT = 0.0f;
			}

			attack4_.rotateSpeed = Lerp(attack4_.MinxRotateSpeed, attack4_.MaxRotateSpeed, attack4_.rotateT);
			if (attack4_.rotateSpeed <= 0.011f) {
				attack4_.rotateSpeed = 0;

			}
			
			worldTransformBody_.rotate_.y += attack4_.rotateSpeed;




			if (attack4_.rotateT <= 0) {
				behaviorTimer_++;
				if (behaviorTimer_ >= attack4_.cooldownTime) {
					behaviorRequest_ = Behavior::kRoot;
					behaviorTimer_ = 0;
				}
			}
		}
	}





}

#pragma endregion // 攻撃4


#pragma region Root

void Enemy::BehaviorRootInitialize()
{
	// 浮遊ギミック
	InitializeFloatingGimmick();
}

void Enemy::BehaviorRootUpdate()
{
	Move(0.00001f, false);


	// 浮遊ギミック
	UpdateFloatingGimmick();
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

void Enemy::Move(float speed, bool flag)
{
	if (DistanceXZ(player_->GetWorldTransform().GetWorldPosition(), worldTransform_.GetWorldPosition()) >= 1) {
		// 回転と移動量の設定
		const float kMoveSpeed = speed; // 移動速度
		// worldTransformBase_.rotation_.y += 0.00f; // 一定量のY軸回転

		// 向いている方向への移動ベクトルの計算
		Vector3 moveDirection = { 0.0f, 0.0f, kMoveSpeed };
		Matrix4x4 rotationMatrix = MakeRotateYMatrix(worldTransform_.rotate_.y);
		moveDirection = Transform(moveDirection, rotationMatrix);

		// ロックオン座標
		Vector3 lockOnPosition = player_->GetWorldTransform().GetWorldPosition();

		// 追跡対象からロックオン対象へのベクトル
		Vector3 sub = Subtract(lockOnPosition, worldTransform_.transform_);

		// Y軸周り角度
		worldTransform_.rotate_.y = std::atan2(sub.x, sub.z);
		if (flag) {

		}
		else {
			worldTransformBody_.rotate_.y = 0;
		}
		oldPos_ = worldTransform_.transform_;

		worldTransform_.transform_ += moveDirection;
	}
}

#pragma endregion // 通常行動

#pragma region Fear

void Enemy::BehaviorFearInitialize()
{

}

void Enemy::BehaviorFearUpdate()
{
	behaviorTimer_++;
	if (behaviorTimer_ >= 180) {
		behaviorRequest_ = Behavior::kRoot;
		behaviorTimer_ = 0;
	}
}

#pragma endregion // 怯み
