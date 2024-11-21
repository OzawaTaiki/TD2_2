#include <MatrixFunction.h>
#include "VectorFunction.h"

#include "Enemy.h"
#include "../Player.h"
#include "../../../Application/Stage/Stage.h"
#include "ParticleManager.h"
#include "TextureManager.h"

#include "imgui.h"
#include "../Collider/CollisionManager.h"

template<typename T>
T Lerp(const T& a, const T& b, float t) {
	return a * (1.0f - t) + b * t;
}

// easeInOutSine 関数 
float easeInOutSine(float t) { 
	return -(cos(M_PI * t) - 1) / 2; 
}

float easyInOutElastic(float t) {
	const float c5 = (2 * (float)M_PI) / 4.5f;
	if (t == 0) {
		return 0.0f;
	}
	else if (t == 1) {
		return 1.0f;
	}
	if (t < 0.5) {
		return -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2.0f;
	}
	else {
		return (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2.0f + 1.0f;
	}
}

void Enemy::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	worldTransformBody_.Initialize();
	worldTransformBody_.parent_ = &worldTransform_;
	worldTransformBody_.transform_ = Vector3{ 0,0,0 };

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
	color_.SetColor(defaultColor_);

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

	InitializeParticleEmitter();

	// コライダーの初期化
	collider_ = std::make_unique<Collider>();
	collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	collider_->SetShape(model_->GetMin(0), model_->GetMax(0));
	collider_->SetAtrribute("enemy");
	collider_->SetMask({ "enemy" });

	collider_->SetGetWorldMatrixFunc([this]() { return worldTransform_.matWorld_; });
	collider_->SetOnCollisionFunc([this]() { OnCollision(); });


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
	ConfigManager::GetInstance()->SetVariable("attack1", "MaxLength", &attack1_.MaxLength);
	ConfigManager::GetInstance()->SetVariable("attack1", "preparationTime", &attack1_.MaxAttackPreparationTime);
	ConfigManager::GetInstance()->SetVariable("attack1", "armSpeed", &attack1_.armSpeed);
	ConfigManager::GetInstance()->SetVariable("attack1", "ToNextPredictionDelay", &attack1_.MaxAttackToNextPredictionDelay);
	ConfigManager::GetInstance()->SetVariable("attack1", "weakArm", &attack1_.weakArmSpawnProbability);
	ConfigManager::GetInstance()->SetVariable("attack1", "armRetractTime", &attack1_.MaxArmRetractTime);
	
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
	float ShotsPerPhase = float(attack3_.MaxNumShotsPerPhase);
	ConfigManager::GetInstance()->SetVariable("attack3", "numShotsPerPhase", &ShotsPerPhase);
	ConfigManager::GetInstance()->SetVariable("attack3", "speed", &attack3_.speed);
	ConfigManager::GetInstance()->SetVariable("attack3", "MaxYTime", &attack3_.MaxYTime);
	ConfigManager::GetInstance()->SetVariable("attack3", "MinYTime", &attack3_.MinYTime);
	ConfigManager::GetInstance()->SetVariable("attack3", "MaxPosY", &attack3_.MaxPosY);
	ConfigManager::GetInstance()->SetVariable("attack3", "MinPosY", &attack3_.MinPosY);

	// 攻撃4
	ConfigManager::GetInstance()->SetVariable("attack4", "MaxRotateSpeed", &attack4_.MaxRotateSpeed);
	ConfigManager::GetInstance()->SetVariable("attack4", "MinRotateSpeed", &attack4_.MinxRotateSpeed);
	ConfigManager::GetInstance()->SetVariable("attack4", "SpinTime", &attack4_.MaxSpinTime);
	ConfigManager::GetInstance()->SetVariable("attack4", "speed", &attack4_.speed);
	ConfigManager::GetInstance()->SetVariable("attack4", "ArmGrowthToSpinDelay", &attack4_.MaxArmGrowthToSpinDelay);
	ConfigManager::GetInstance()->SetVariable("attack4", "StoppingTime", &attack4_.MaxStoppingTime);
	ConfigManager::GetInstance()->SetVariable("attack4", "cooldownTime", &attack4_.cooldownTime);


	// hitcolorの設定
    ConfigManager::GetInstance()->SetVariable("enemy", "defaultColor", &defaultColor_);
    ConfigManager::GetInstance()->SetVariable("enemy", "hitColor", &hitColor_);
    ConfigManager::GetInstance()->SetVariable("enemy", "hitColorMaxTime", &hitColorMaxTime_);



	srand(unsigned int(time(nullptr))); // シードを現在の時刻で設定
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
			ImGui::Checkbox("debugAttack", &isDebugAttack);
			if (ImGui::Button("SpecialAttack")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kSpecial;
				specialAttackBehaviorRequest_ = SpecialAttack::kAttack;
			}
			if (ImGui::Button("SpecialAttack2")) {
				behaviorTimer_ = 0;

				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kSpecial;
				specialAttackBehaviorRequest_ = SpecialAttack::kAttack2;
			}
			if (ImGui::Button("SpecialAttack3")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kSpecial;
				specialAttackBehaviorRequest_ = SpecialAttack::kAttack3;
			}
			if (ImGui::Button("SpecialAttack4")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kSpecial;
				specialAttackBehaviorRequest_ = SpecialAttack::kAttack4;
			}
			if (ImGui::Button("NormalLongAttack1")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kNormal;
				normalAttackBehaviorRequest_ = NormalAttack::kAttackLong1;
			}
			if (ImGui::Button("NormalLongAttack2")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kNormal;
				normalAttackBehaviorRequest_ = NormalAttack::kAttackLong2;
			}
      if (ImGui::TreeNode("HitColor"))
      {
          ImGui::ColorEdit4("defaultColor", &defaultColor_.x);
          ImGui::ColorEdit4("hitColor", &hitColor_.x);
          ImGui::DragFloat("hitColorMaxTime", &hitColorMaxTime_, 0.01f);
          if (ImGui::Button("save hitColor"))
          {
              ConfigManager::GetInstance()->SaveData("enemy");
          }
          ImGui::TreePop();
      }
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (ImGui::BeginTabBar("Boss"))
	{
		if (ImGui::BeginTabItem("worldTransfom"))
		{
			ImGui::DragFloat3("worldTransform_.transform_", &worldTransform_.transform_.x, 0.01f);
			ImGui::DragFloat3("worldTransformBody_.transform_", &worldTransformBody_.transform_.x, 0.01f);
			ImGui::DragFloat3("worldTransformLeft_.transform_", &worldTransformLeft_.transform_.x, 0.01f);
			ImGui::DragFloat3("worldTransformRight_.transform_", &worldTransformRight_.transform_.x, 0.01f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("attack1"))
		{
			ImGui::DragFloat3("pos", &attack1_.attackPos.x, 0.01f);
			ImGui::DragFloat("armSpeed", &attack1_.armSpeed, 0.01f);
			ImGui::DragInt("armNum", &attack1_.MaxArmNum, 0.01f);
			ImGui::DragFloat("attackPower", &attack1_.attackPower, 0.01f);
			ImGui::DragFloat("poreparationTime", &attack1_.MaxAttackPreparationTime, 0.01f);
			ImGui::DragFloat("MaxLength", &attack1_.MaxLength, 0.01f);
			ImGui::DragFloat("armRetractTime", &attack1_.MaxArmRetractTime, 0.01f);
			ImGui::DragFloat("ToNextPredictionDelay", &attack1_.MaxAttackToNextPredictionDelay, 0.01f);
			ImGui::DragFloat("LandingTime", &attack1_.MaxLandingTime, 0.01f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("attack2"))
		{
			//ImGui::DragFloat3("pos", &attack2_.attackPos.x, 0.01f);
			ImGui::DragFloat("ExpandTime", &attack2_.MaxExpandTime, 0.01f);
			//ImGui::DragFloat("thicknessStartTime", &attack2_.MaxThicknessStartTime, 0.01f);
			ImGui::DragFloat("attackPower", &attack2_.attackPower, 0.01f);
			ImGui::DragFloat("poreparationTime", &attack2_.MaxAttackPreparationTime, 0.01f);
			//ImGui::DragFloat("thunderStrikeTime", &attack2_.MaxThunderStrikeTime, 0.01f);
			ImGui::DragFloat("maxSize", &attack2_.maxSize, 0.01f);
			ImGui::DragFloat("minSize", &attack2_.minSize, 0.01f);
			//ImGui::DragFloat("LandingTime", &attack2_.MaxLandingTime, 0.01f);
			ImGui::DragFloat("MaxDeathTimer_", &attack2_.MaxDeathTimer_, 0.01f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("attack3"))
		{
			//ImGui::DragFloat3("pos", &attack3_.attackPos.x, 0.01f);
			ImGui::DragFloat("cooldown", &attack3_.MaxAttackCooldown, 0.01f);
			ImGui::DragFloat("MinPosY", &attack3_.MinPosY, 0.01f);
			ImGui::DragFloat("MaxPosY", &attack3_.MaxPosY, 0.01f);
			ImGui::DragFloat("MinYTime", &attack3_.MinYTime, 0.01f);
			ImGui::DragFloat("MaxYTime", &attack3_.MaxYTime, 0.01f);
			ImGui::DragInt("numShotsPerPhase", &attack3_.MaxNumShotsPerPhase, 1.0f);
			ImGui::DragFloat("attackPower", &attack3_.attackPower, 0.01f);
			ImGui::DragFloat("speed", &attack3_.speed, 0.01f);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("attack4"))
		{
			ImGui::DragFloat("ArmGrowthToSpinDelay", &attack4_.MaxArmGrowthToSpinDelay, 0.01f);
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

	StageMovementRestrictions();

	// エミッターの更新
    UpdateParticleEmitter();

	// ワールドトランスフォーム更新
	worldTransform_.UpdateData();
	worldTransformLeft_.UpdateData();
	worldTransformRight_.UpdateData();
	worldTransformBody_.UpdateData();

	attackCamera_.UpdateMatrix();
	attackCamera2_.UpdateMatrix();
	attackCamera3_.UpdateMatrix();

	if(isAlive)
	{
		UpdateHitColor();
		CollisionManager::GetInstance()->RegisterCollider(collider_.get());
	}

}

void Enemy::Draw(const Camera& camera)
{
	model_->Draw(worldTransformBody_, &camera, &color_);

	for (const auto& bullet : bullets_) {
		bullet->Draw(camera);
	}

	for (const auto& bullet : normalbullets_) {
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

		// 攻撃遷移
		switch (attackBehavior_) {
		case AttackBehavior::kNormal: // 通常攻撃の場合
			switch (normalAttackBehavior_)
			{
			case Enemy::NormalAttack::kAttackShort1: // 近距離1
				break;
			case Enemy::NormalAttack::kAttackShort2: // 近距離2
				break;
			case Enemy::NormalAttack::kAttackLong1:  // 遠距離1
				break;
			case Enemy::NormalAttack::kAttackLong2:  // 遠距離2
				break;
			default:
				break;
			}
			break;
		case AttackBehavior::kSpecial: // 必殺技攻撃の場合
			switch (specialAttackBehavior_)
			{
			case Enemy::SpecialAttack::kAttack:		// 攻撃1
				break;
			case Enemy::SpecialAttack::kAttack2:	// 攻撃2
				break;
			case Enemy::SpecialAttack::kAttack3:	// 攻撃3
				break;
			case Enemy::SpecialAttack::kAttack4:	// 攻撃4
				modelLeftArm_->Draw(worldTransformLeft_, &camera, &color_);
				modelRightArm_->Draw(worldTransformRight_, &camera, &color_);
				break;
			default:
				break;
			}
			break;
		}
		break;
	}

    // エミッターの描画
	for (uint32_t index = 0; index < particleEmitter_.size(); ++index)
        particleEmitter_[index].Draw();

    collider_->Draw();

}

void Enemy::OnCollision()
{
    color_.SetColor(hitColor_);
	isHitColor_ = true;
}

void Enemy::StageMovementRestrictions()
{
	if (stage_->GetWallBack().z < worldTransform_.GetWorldPosition().z) {
		worldTransform_.transform_.z = stage_->GetWallBack().z;
	}
	else if (stage_->GetWallFlont().z > worldTransform_.GetWorldPosition().z) {
		worldTransform_.transform_.z = stage_->GetWallFlont().z;
	}
	if (stage_->GetWallLeft().x > worldTransform_.GetWorldPosition().x) {
		worldTransform_.transform_.x = stage_->GetWallLeft().x;
	}
	else if (stage_->GetWallRight().x < worldTransform_.GetWorldPosition().x) {
		worldTransform_.transform_.x = stage_->GetWallRight().x;
	}

}


#pragma region 状態遷移

#pragma region Root

void Enemy::BehaviorRootInitialize()
{
	// 浮遊ギミック
	InitializeFloatingGimmick();

	srand(unsigned int(time(nullptr))); // シードを現在の時刻で設定
	// 不規則な移動数
	rootMove_.MaxNumMovePhase = rand() % rootMove_.MaxRandMovePhase + 1;
	rootMove_.isBehavior_ = false;
	rootMove_.startPos = worldTransform_.transform_;
	rootMove_.targetPos.x = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	rootMove_.targetPos.z = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	rootMove_.targetPos.y = worldTransform_.transform_.y;
	rootMove_.transitionFactor = 0;
	rootMove_.numMovePhase = 0;
	rootMove_.coolTime = 0;
}

void Enemy::BehaviorRootUpdate()
{
	//float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	// 行動遷移しない
	if (rootMove_.isBehavior_ == false) {
		// フェーズを繰り返す
		if (rootMove_.numMovePhase <= rootMove_.MaxNumMovePhase) {
			// 移動

			rootMove_.transitionFactor += rootMove_.transitionSpeed;


			if (rootMove_.transitionFactor >= 1.0f) {

				if (++rootMove_.coolTime >= rootMove_.MaxCoolTime) {
					// 0に

					// クールタイムを0に
					rootMove_.coolTime = 0;
					// 始点を現在の位置に
					rootMove_.startPos = worldTransform_.transform_;

					// 終点をランダムに(一旦)
					rootMove_.targetPos.x = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
					rootMove_.targetPos.z = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
					rootMove_.targetPos.y = worldTransform_.transform_.y;
					rootMove_.MaxCoolTime = float(rand() % rootMove_.MaxRandCoolTime + 10);

					rootMove_.transitionSpeed = float(float(float(rand() % 100 + 100) / 10000.0f));

					//フェーズを加算
					rootMove_.numMovePhase++;
					rootMove_.transitionFactor = 0.0f;
				}
				else {
					rootMove_.transitionFactor = 1.0f;
				}
			}
			else {
				// 移動
				worldTransform_.transform_ = Lerp(rootMove_.startPos, rootMove_.targetPos, easeInOutSine(rootMove_.transitionFactor));
				Move(0.0f, false);
			}
		}
		else {
			rootMove_.isBehavior_ = true;
		}
	}
	else { // 行動遷移する

		Move(0.01f, false);

		if (!isDebugAttack) {
			behaviorTimer_++;
			if (behaviorTimer_ >= 120) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
			}
		}
	}

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

#pragma region Attack

void Enemy::BehaviorAttackInitialize()
{

	if (!isDebugAttack) {
		if (20 >= DistanceXZ(worldTransform_.GetWorldPosition(), player_->GetWorldTransform().GetWorldPosition())) {
			attackBehaviorRequest_ = AttackBehavior::kSpecial;

			specialAttackBehaviorRequest_ = SpecialAttack::kAttack4;

		}
		else {
			attackBehaviorRequest_ = AttackBehavior::kSpecial;

			specialAttackBehaviorRequest_ = SpecialAttack::kAttack3;

		}
	}


	if (attackBehaviorRequest_) {
		// ふるまいを変更する
		attackBehavior_ = attackBehaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (attackBehavior_) {
		case AttackBehavior::kNormal: // 通常攻撃の場合

			if (normalAttackBehaviorRequest_) {
				// ふるまい
				normalAttackBehavior_ = normalAttackBehaviorRequest_.value();
				switch (normalAttackBehavior_)
				{
				case Enemy::NormalAttack::kAttackShort1: // 近距離1
					NormalShotAttack1Initialize();
					break;
				case Enemy::NormalAttack::kAttackShort2: // 近距離2
					NormalShotAttack2Initialize();
					break;
				case Enemy::NormalAttack::kAttackLong1:  // 遠距離1
					NormalLongAttack1Initialize();
					break;
				case Enemy::NormalAttack::kAttackLong2:  // 遠距離2
					NormalLongAttack2Initialize();
					break;
				default:
					break;
				}
				normalAttackBehaviorRequest_ = std::nullopt;
			}
			break;
		case AttackBehavior::kSpecial: // 必殺技攻撃の場合

			if (specialAttackBehaviorRequest_) {
				specialAttackBehavior_ = specialAttackBehaviorRequest_.value();

				switch (specialAttackBehavior_)
				{
				case Enemy::SpecialAttack::kAttack:		// 攻撃1
					SpecialAttackInitialize();
					break;
				case Enemy::SpecialAttack::kAttack2:	// 攻撃2
					SpecialAttack2Initialize();
					break;
				case Enemy::SpecialAttack::kAttack3:	// 攻撃3
					SpecialAttack3Initialize();
					break;
				case Enemy::SpecialAttack::kAttack4:	// 攻撃4
					SpecialAttack4Initialize();
					break;
				default:
					break;
				}

				specialAttackBehaviorRequest_ = std::nullopt;
			}
			break;
		}
		// ふるまいリクエストリセット
		attackBehaviorRequest_ = std::nullopt;
	}
}

void Enemy::BehaviorAttackUpdate()
{

	// 攻撃遷移
	switch (attackBehavior_) {
	case AttackBehavior::kNormal: // 通常攻撃の場合
		switch (normalAttackBehavior_)
		{
		case Enemy::NormalAttack::kAttackShort1: // 近距離1
			NormalShotAttack1Update();
			break;
		case Enemy::NormalAttack::kAttackShort2: // 近距離2
			NormalShotAttack2Update();
			break;
		case Enemy::NormalAttack::kAttackLong1:  // 遠距離1
			NormalLongAttack1Update();
			break;
		case Enemy::NormalAttack::kAttackLong2:  // 遠距離2
			NormalLongAttack2Update();
			break;
		default:
			break;
		}
		break;
	case AttackBehavior::kSpecial: // 必殺技攻撃の場合
		switch (specialAttackBehavior_)
		{
		case Enemy::SpecialAttack::kAttack:		// 攻撃1
			SpecialAttackUpdate();
			break;
		case Enemy::SpecialAttack::kAttack2:	// 攻撃2
			SpecialAttack2Update();
			break;
		case Enemy::SpecialAttack::kAttack3:	// 攻撃3
			SpecialAttack3Update();
			break;
		case Enemy::SpecialAttack::kAttack4:	// 攻撃4
			SpecialAttack4Update();
			break;
		default:
			break;
		}
		break;
	}
}

#pragma endregion // 攻撃行動

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

#pragma endregion // 怯み行動

#pragma endregion // 大まかな状態


#pragma region Special

#pragma region Attack1

void Enemy::SpecialAttackInitialize()
{
	attack1_.isBulletShot = false;
	attack1_.clock1 = 1;
	attack1_.transitionFactor = 0;

	attack1_.armNum = 0;
	attack1_.landingTime = 0;

}

void Enemy::SpecialAttackUpdate()
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

#pragma endregion // 攻撃1

#pragma region Attack2

void Enemy::ThunderInitialize(Vector3 pos)
{
	const float kBulletSpeed = 0.4f;
	Vector3 velocityB{};
	//Vector3 direction{ };



	for (int x = 0; x < attack2_.num; x++) {
		for (int z = 0; z < attack2_.num; z++) {
			Vector3 pospos{};
			Vector3 direction{};
			pospos.x = float(x * attack2_.positionInterval) - ((attack2_.num - 1) * (attack2_.positionInterval / 2));
			pospos.y = stage_->GetWallFloor().y;
			pospos.z = float(z * attack2_.positionInterval) - ((attack2_.num - 1) * (attack2_.positionInterval / 2));

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

void Enemy::SpecialAttack2Initialize()
{
	attack2_.isBulletShot = false;
	attack2_.clock = 1;
	attack2_.transitionFactor = 0;
	attack2_.deathTimer_ = 0;
	attack2_.landingTime = 0;
}

void Enemy::SpecialAttack2Update()
{
	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	Vector3 targetPos;
	Vector3 pos{ 0,0,0 };
	Vector3 startPos;
	if (attack2_.isBulletShot == false) {


		if (attack2_.clock == 1) {
			if (attack2_.transitionFactor == 0) {

				ThunderInitialize(pos);


			}
			// 補間の進行度を更新
			if (attack2_.transitionFactor < 1.0f) {
				attack2_.transitionFactor += transitionSpeed;
			}
			else {
				attack2_.transitionFactor = 0.0f; // 補間が完了したら1.0fで固定
				attack2_.clock *= -1;
				attack2_.isBulletShot = false;
			}



			startPos = worldTransform_.transform_;
			startPos.y = attack2_.attackMinPos;

			targetPos = worldTransform_.transform_;
			targetPos.y = attack2_.attackMaxPos;


			worldTransform_.transform_ = Lerp(startPos, targetPos, attack2_.transitionFactor);
		}
		if (attack2_.clock == -1) {
			// 補間の進行度を更新
			if (attack2_.transitionFactor < 1.0f) {
				attack2_.transitionFactor += transitionSpeed * 6;
			}
			else {
				attack2_.transitionFactor = 1.0f;
				attack2_.isBulletShot = true;
			}

			startPos = worldTransform_.transform_;
			startPos.y = attack2_.attackMaxPos;


			targetPos = worldTransform_.transform_;
			targetPos.y = attack2_.attackMinPos;

			worldTransform_.transform_ = Lerp(startPos, targetPos, attack2_.transitionFactor);


		}
	}


	if (attack2_.isBulletShot == true) {
		behaviorTimer_++;
		if (behaviorTimer_ >= 60) {
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

	// 
	for (const auto& bullet : normalbullets_) {
		bullet->Update();
	}
	// デスフラグが立った弾を削除
	normalbullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) { return bullet->IsDead(); });


	for (const auto& bullet : thunder_) {
		bullet->Update();
	}
	// デスフラグが立った弾を削除
	thunder_.remove_if([](const std::unique_ptr<EnemyThunder>& bullet) { return bullet->IsDead(); });
}

void Enemy::SpecialAttack3Initialize()
{
	attack3_.isBulletShot = false;
	attack3_.clock1 = 1;
	attack3_.transitionFactor = 0;

	attack3_.numShotsPerPhase = 0;
	attack3_.attackCooldown = attack3_.MaxAttackCooldown;
	attack3_.minYTime = 0;
	attack3_.maxYTime = 0;
	//attack3_.
	behaviorTimer_ = 0;
}

void Enemy::SpecialAttack3Update() {

	float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）


	Vector3 targetPos;
	Vector3 startPos;
	if (attack3_.isBulletShot == false) {
		if (attack3_.clock1 == 1) {
			targetPos.x = worldTransform_.transform_.x;
			targetPos.y = attack3_.MaxPosY;
			targetPos.z = worldTransform_.transform_.z;


			startPos = worldTransform_.transform_;
			startPos.y = attack3_.MinPosY;

			if (attack3_.transitionFactor < 1.0f) {
				attack3_.transitionFactor += transitionSpeed * 5.0f;
			}
			else {
				attack3_.transitionFactor = 1.0f; // 補間が完了したら1.0fで固定
				attack3_.isBulletShot = false;

				if (++attack3_.maxYTime >= attack3_.MaxYTime) {
					attack3_.clock1 *= -1;
					attack3_.transitionFactor = 0.0f;
				}
			}


			worldTransform_.transform_ = StartEnd(startPos, targetPos, attack3_.transitionFactor);
		}
		if (attack3_.clock1 == -1) {
			targetPos.y = attack3_.MinPosY;
			targetPos.x = worldTransform_.transform_.x;
			targetPos.z = worldTransform_.transform_.z;


			startPos = worldTransform_.transform_;
			startPos.y = attack3_.MaxPosY;

			// 補間の進行度を更新
			if (attack3_.transitionFactor < 1.0f) {
				attack3_.transitionFactor += transitionSpeed * 10.0f;

			}
			else {
				attack3_.isBulletShot = true;
				attack3_.transitionFactor = 1.0f;
				attack3_.clock1 *= -1;
			}



			worldTransform_.transform_ = StartEnd(startPos, targetPos, attack3_.transitionFactor);
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

void Enemy::SpecialAttack4Initialize()
{
	attack4_.transitionFactor = 0;
	worldTransformLeft_.transform_ = { 0,0,0 };
	worldTransformRight_.transform_ = { 0,0,0 };
	attack4_.spinTime = 0;
	attack4_.armGrowthToSpinDelay = 0;
	attack4_.stoppingTime = 0;
	attack4_.recoilTime = 0;
}

void Enemy::SpecialAttack4Update()
{
	static float transitionFactor = 0.01f;

	attack4_.transitionFactor += transitionFactor;

	float str = 0;
	float end = 4;
	float endm = -4;
	if (++attack4_.armGrowthToSpinDelay <= attack4_.MaxArmGrowthToSpinDelay) {
		worldTransformLeft_.transform_.x = StartEnd(str, endm, attack4_.transitionFactor);
		worldTransformRight_.transform_.x = StartEnd(str, end, attack4_.transitionFactor);
		if (worldTransformLeft_.transform_.x >= endm) {
			worldTransformBody_.rotate_.y -= 0.01f;
		}
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

void Enemy::InitializeParticleEmitter()
{
	particleEmitter_[0].Setting("enemyFloating0");
	particleEmitter_[1].Setting("enemyFloating1");
	particleEmitter_[2].Setting("enemyFloating2");

	uint32_t th = TextureManager::GetInstance()->Load("circle.png");
	for (uint32_t index = 0; index < particleEmitter_.size(); ++index)
	{
		ParticleManager::GetInstance()->CreateParticleGroup(particleEmitter_[index].GetName(), "plane/plane.obj", &particleEmitter_[index], th);
        particleEmitter_[index].SetWorldMatrix(&worldTransform_.matWorld_);
		particleEmitter_[index].SetEmit(1);
	}
}

void Enemy::UpdateParticleEmitter()
{
	for (uint32_t index = 0; index < particleEmitter_.size(); ++index)
	{
		particleEmitter_[index].Update();
	}
}

void Enemy::UpdateHitColor()
{
	if (isHitColor_)
	{
		hitColorTimer_ += 1.0f / 60.0f;
		if (hitColorTimer_ >= hitColorMaxTime_)
		{
			hitColorTimer_ = 0;
			isHitColor_ = false;
			color_.SetColor(defaultColor_);
		}
	}
}

#pragma endregion // 攻撃4

#pragma endregion //必殺技


#pragma region Noraml

void Enemy::NormalShotAttack1Initialize()
{
}

void Enemy::NormalShotAttack1Update()
{
}

void Enemy::NormalShotAttack2Initialize()
{
}

void Enemy::NormalShotAttack2Update()
{
}

#pragma region NormalLongAttack

void Enemy::NormalBulletInitialize(Vector3 pos)
{
	const float kBulletSpeed = normalAttackBullet_.speed;
	Vector3 velocityB{};

	//const int numBullets = normalAttackBullet_.numElectricCount;


	//Vector3 direction{ cosf(radian + attack3_.numShotsPerPhase) + pos.x, pos.y, sinf(radian + attack3_.numShotsPerPhase) + pos.z };

	velocityB = Subtract(normalAttackBullet_.oldTraget, pos);
	velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

	// 弾を生成し、初期化
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(pos, velocityB, modelBullet_, &attack3_);

	// 弾の親設定
	newBullet->SetParent(worldTransform_.parent_);

	// 弾を登録する
	bullets_.push_back(std::move(newBullet));

}

void Enemy::NormalLongAttack1Initialize()
{
	normalAttackBullet_.isBulletShot = false;
	normalAttackBullet_.clock1 = 1;
	normalAttackBullet_.transitionFactor = 0;

	normalAttackBullet_.numShotsPerPhase = 0;
	normalAttackBullet_.MaxAttackCooldown = 10.0f;
	normalAttackBullet_.attackCooldown = normalAttackBullet_.MaxAttackCooldown;
	normalAttackBullet_.minYTime = 0;
	normalAttackBullet_.maxYTime = 0;

	normalAttackBullet_.MaxNumShotsPerPhase = 5;
	normalAttackBullet_.oldTraget = player_->GetWorldTransform().GetWorldPosition();
	normalAttackBullet_.speed = 0.7f;
	//attack3_.
	behaviorTimer_ = 0;
}

void Enemy::NormalLongAttack1Update()
{
	if (normalAttackBullet_.numShotsPerPhase < normalAttackBullet_.MaxNumShotsPerPhase) {
		if (++normalAttackBullet_.attackCooldown >= normalAttackBullet_.MaxAttackCooldown) {
			NormalBulletInitialize(worldTransform_.GetWorldPosition());
			normalAttackBullet_.numShotsPerPhase++;
			normalAttackBullet_.attackCooldown = 0;
		}
	}
	if (normalAttackBullet_.numShotsPerPhase >= normalAttackBullet_.MaxNumShotsPerPhase) {
		behaviorTimer_++;
	}

	if (behaviorTimer_ >= 120) {
		behaviorRequest_ = Behavior::kRoot;
		behaviorTimer_ = 0;
	}
}

#pragma endregion // 通常遠距離攻撃1

#pragma region NormalLong2Attack

void Enemy::Normal2BulletInitialize(Vector3 pos)
{
	const float kBulletSpeed = normal2AttackBullet_.speed;
	Vector3 velocityB{};

	const int numBullets = normal2AttackBullet_.numElectricCount;
	const float angleStep = 360.0f / numBullets;

	for (int i = 0; i < numBullets; i++) {
		float angle = i * angleStep;
		float radian = angle * (3.14f / 180.0f);  // Convert to radians

		float rotate = float(normal2AttackBullet_.numShotsPerPhase / 3000);

		Vector3 direction{ cosf(radian + normal2AttackBullet_.numShotsPerPhase) + pos.x, pos.y, sinf(radian + normal2AttackBullet_.numShotsPerPhase) + pos.z };

		velocityB = Subtract(direction, pos);
		velocityB = Multiply(Normalize(velocityB), kBulletSpeed);

		// 弾を生成し、初期化
		auto newBullet = std::make_unique<EnemyBullet>();
		newBullet->Initialize(pos, velocityB, modelBullet_, &normal2AttackBullet_);

		// 弾の親設定
		newBullet->SetParent(worldTransform_.parent_);

		// 弾を登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Enemy::NormalLongAttack2Initialize()
{
	normal2AttackBullet_.isBulletShot = false;
	normal2AttackBullet_.clock1 = 1;
	normal2AttackBullet_.transitionFactor = 0;

	normal2AttackBullet_.numShotsPerPhase = 0;
	normal2AttackBullet_.attackCooldown = normal2AttackBullet_.MaxAttackCooldown;
	normal2AttackBullet_.minYTime = 0;
	normal2AttackBullet_.maxYTime = 0;


	// 最大値
	normal2AttackBullet_.MaxNumShotsPerPhase = 2;
	// 速度
	normal2AttackBullet_.speed = 0.5f;
	behaviorTimer_ = 0;
}

void Enemy::NormalLongAttack2Update()
{
	if (normal2AttackBullet_.numShotsPerPhase < normal2AttackBullet_.MaxNumShotsPerPhase) {
		if (++normal2AttackBullet_.attackCooldown >= normal2AttackBullet_.MaxAttackCooldown) {
			Normal2BulletInitialize(worldTransform_.GetWorldPosition());
			normal2AttackBullet_.numShotsPerPhase++;
			normal2AttackBullet_.attackCooldown = 0;
		}
	}
	if (normal2AttackBullet_.numShotsPerPhase >= normal2AttackBullet_.MaxNumShotsPerPhase) {
		behaviorTimer_++;
	}

	if (behaviorTimer_ >= 120) {
		behaviorRequest_ = Behavior::kRoot;
		behaviorTimer_ = 0;
	}
}

#pragma endregion // 通常遠距離攻撃2

#pragma endregion 通常攻撃




