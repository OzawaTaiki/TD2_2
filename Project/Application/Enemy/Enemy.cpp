
#include "Enemy.h"
#include "../Player.h"
#include "../../../Application/Stage/Stage.h"
#include "ParticleManager.h"
#include "TextureManager.h"
#include "../FollowCamera.h"
#include "ImGuiManager.h"
#include "CollisionManager.h"

#include "random"


template<typename T>
T Lerp(const T& a, const T& b, float t) {
	return a * (1.0f - t) + b * t;
}

//easeInOutSine 関数
float easeInOutSine(float t) {
	return -(cosf(float(M_PI) * t) - 1) / 2;

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

// 最短角度での線形補間
float ShortestAngleLerp(float start, float end, float factor) {
	float difference = fmod(end - start, 360.0f); if (difference > 180.0f) {
		difference -= 360.0f;
	}
	else if (difference < -180.0f) {
		difference += 360.0f;
	} return start + factor * difference;
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

	modelBullet_ = Model::CreateFromObj("electricBall/electricBall.obj");
	modelStageArm_ = Model::CreateFromObj("bossAtackArm/bossAtackArm.obj");
	modelThunder_ = Model::CreateFromObj("PredictionBox/PredictionSphere.obj");



	// 移動予測線
	worldPrediction_.Initialize();

	modelPrediction_ = Model::CreateFromObj("PredictionBox/PredictionPlaneBox.obj");;

	colorPrediction_.Initialize();
	colorPrediction_.SetColor({ 1, 0.271f, 0,1 });

	InitializeParticleEmitter();

	// コライダーの初期化
	bodyCollider_ = std::make_unique<Collider>();
	bodyCollider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	bodyCollider_->SetShape(model_->GetMin(), model_->GetMax());
	bodyCollider_->SetAtrribute("enemy");
	bodyCollider_->SetMask({ "enemy","enemyLeft", "enemyRight","enemyBullet" });

	bodyCollider_->SetGetWorldMatrixFunc([this]() { return worldTransform_.matWorld_; });
	bodyCollider_->SetOnCollisionFunc([this](const Collider* _other) { OnCollision(_other); });

    // 左手コライダーの初期化
    leftArmCollider_ = std::make_unique<Collider>();
    leftArmCollider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
    leftArmCollider_->SetShape(modelLeftArm_->GetMin(), modelLeftArm_->GetMax());
    leftArmCollider_->SetAtrribute("enemyLeft");
    leftArmCollider_->SetMask({ "enemy","enemyLeft", "enemyRight","enemyBullet"});
    leftArmCollider_->SetGetWorldMatrixFunc([this]() { return worldTransformLeft_.matWorld_; });
    leftArmCollider_->SetOnCollisionFunc([this](const Collider* _other) { OnCollision(_other); });

    // 右手コライダーの初期化
    rightArmCollider_ = std::make_unique<Collider>();
    rightArmCollider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
    rightArmCollider_->SetShape(modelRightArm_->GetMin(), modelRightArm_->GetMax());
    rightArmCollider_->SetAtrribute("enemyRight");
    rightArmCollider_->SetMask({ "enemy","enemyLeft", "enemyRight","enemyBullet" });
    rightArmCollider_->SetGetWorldMatrixFunc([this]() { return worldTransformRight_.matWorld_; });
    rightArmCollider_->SetOnCollisionFunc([this](const Collider* _other) { OnCollision(_other); });


	deashParticle_ = std::make_unique<EnemyDeathParticle>();
	deashParticle_->Initialize("DeathParticle");
	deashParticle_->SetPlayerMat(&worldTransform_);

	deashExplosionParticle_ = std::make_unique<EnemyDeathParticle>();
	deashExplosionParticle_->Initialize("DeathExplosionParticle");
	deashExplosionParticle_->SetPlayerMat(&worldTransform_);




	for (uint32_t index = 0; index < deashSmokeParticle_.size(); ++index)
	{
		deashSmokeParticle_[index] = std::make_unique<EnemyDeathParticle>();
		std::string str = "enemySmoke" + std::to_string(index);
		deashSmokeParticle_[index]->Initialize(str);
		deashSmokeParticle_[index]->SetPlayerMat(&worldTransform_);
	}

#pragma region ConfigManager
    ConfigManager* configManager = ConfigManager::GetInstance();

	configManager->SetVariable("enemy", "MaxHp", &MaxHp);

	configManager->SetVariable("root", "MaxRandCoolTime", &rootMove_.MaxRandCoolTime);
	configManager->SetVariable("root", "MaxRandMovePhase", &rootMove_.MaxRandMovePhase);
	configManager->SetVariable("root", "MaxCoolTime", &rootMove_.MaxCoolTime);
	// 通常行動


	// 攻撃1
	configManager->SetVariable("attack1", "attackPos", &attack1_.attackPos);
	configManager->SetVariable("attack1", "attackPower", &attack1_.attackPower);
	configManager->SetVariable("attack1", "andingTime", &attack1_.MaxLandingTime);
	float armmax = float(attack1_.MaxArmNum);
	configManager->SetVariable("attack1", "armNum", &armmax);
	configManager->SetVariable("attack1", "MaxLength", &attack1_.MaxLength);
	configManager->SetVariable("attack1", "preparationTime", &attack1_.MaxAttackPreparationTime);
	configManager->SetVariable("attack1", "armSpeed", &attack1_.armSpeed);
	configManager->SetVariable("attack1", "ToNextPredictionDelay", &attack1_.MaxAttackToNextPredictionDelay);
	configManager->SetVariable("attack1", "weakArm", &attack1_.weakArmSpawnProbability);
	configManager->SetVariable("attack1", "armRetractTime", &attack1_.MaxArmRetractTime);

	// 攻撃2
	configManager->SetVariable("attack2", "attackPreparationTime", &attack2_.MaxAttackPreparationTime);
	configManager->SetVariable("attack2", "attackPower", &attack2_.attackPower);
	configManager->SetVariable("attack2", "landingTime", &attack2_.MaxLandingTime);
	configManager->SetVariable("attack2", "expandTime", &attack2_.MaxExpandTime);
	configManager->SetVariable("attack2", "thicknessStartTime", &attack2_.MaxThicknessStartTime);
	configManager->SetVariable("attack2", "thunderStrikeTime", &attack2_.MaxThunderStrikeTime);
	configManager->SetVariable("attack2", "maxSize", &attack2_.maxSize);
	configManager->SetVariable("attack2", "minSize", &attack2_.minSize);

	// 攻撃3
	configManager->SetVariable("attack3", "cooldown", &attack3_.MaxAttackCooldown);
	configManager->SetVariable("attack3", "attackPower", &attack3_.attackPower);
	float ShotsPerPhase = float(attack3_.MaxNumShotsPerPhase);
	configManager->SetVariable("attack3", "numShotsPerPhase", &ShotsPerPhase);
	configManager->SetVariable("attack3", "speed", &attack3_.speed);
	configManager->SetVariable("attack3", "MaxYTime", &attack3_.MaxYTime);
	configManager->SetVariable("attack3", "MinYTime", &attack3_.MinYTime);
	configManager->SetVariable("attack3", "MaxPosY", &attack3_.MaxPosY);
	configManager->SetVariable("attack3", "MinPosY", &attack3_.MinPosY);
	configManager->SetVariable("attack3", "Cooldown", &attack3_.MaxAttackCooldown);

	configManager->SetVariable("attack3", "ElectricCount", &attack3_.numElectricCount);

	// 攻撃4
	configManager->SetVariable("attack4", "MaxRotateSpeed", &attack4_.MaxRotateSpeed);
	configManager->SetVariable("attack4", "MinRotateSpeed", &attack4_.MinxRotateSpeed);
	configManager->SetVariable("attack4", "SpinTime", &attack4_.MaxSpinTime);
	configManager->SetVariable("attack4", "speed", &attack4_.speed);
	configManager->SetVariable("attack4", "ArmGrowthToSpinDelay", &attack4_.MaxArmGrowthToSpinDelay);
	configManager->SetVariable("attack4", "AttackPower", &attack4_.attackPower);
	//configManager->SetVariable("attack4", "StoppingTime", &attack4_.MaxStoppingTime);
	configManager->SetVariable("attack4", "cooldownTime", &attack4_.cooldownTime);

	// 通常近距離攻撃1
	configManager->SetVariable("normalAttackShot1_", "MaxRotateSpeed", &normalAttackShot1_.MaxRotateSpeed);
	configManager->SetVariable("normalAttackShot1_", "MinRotateSpeed", &normalAttackShot1_.MinxRotateSpeed);
	configManager->SetVariable("normalAttackShot1_", "SpinTime", &normalAttackShot1_.MaxSpinTime);
	configManager->SetVariable("normalAttackShot1_", "speed", &normalAttackShot1_.speed);
	configManager->SetVariable("normalAttackShot1_", "ArmGrowthToSpinDelay", &normalAttackShot1_.MaxArmGrowthToSpinDelay);
	//configManager->SetVariable("normalAttackShot1_", "StoppingTime", &normalAttackShot1_.MaxStoppingTime);
	configManager->SetVariable("normalAttackShot1_", "cooldownTime", &normalAttackShot1_.cooldownTime);

	// 通常近距離攻撃2
	configManager->SetVariable("normalAttackShot2_", "MaxAssaultTime", &normalAttackShot2_.MaxAssaultTime);
	configManager->SetVariable("normalAttackShot2_", "speed", &normalAttackShot2_.speed);
	configManager->SetVariable("normalAttackShot2_", "ArmGrowthToSpinDelay", &normalAttackShot2_.MaxArmGrowthToSpinDelay);
	configManager->SetVariable("normalAttackShot2_", "StoppingTime", &normalAttackShot2_.MaxStoppingTime);
	configManager->SetVariable("normalAttackShot2_", "cooldownTime", &normalAttackShot2_.cooldownTime);
	configManager->SetVariable("normalAttackShot2_", "rotateSpeed", &normalAttackShot2_.transitionFactorSpeed);


	// 通常攻撃確率
	configManager->SetVariable("attack", "probabilityPhase1", &atMethod_.probabilityPhase1);
	configManager->SetVariable("attack", "probabilityPhase2", &atMethod_.probabilityPhase1);
	configManager->SetVariable("attack", "probabilityPhase3", &atMethod_.probabilityPhase1);
	configManager->SetVariable("attack", "distanceSwich", &atMethod_.distanceSwich);


	// hitcolorの設定
	configManager->SetVariable("enemy", "defaultColor", &defaultColor_);
	configManager->SetVariable("enemy", "hitColor", &hitColor_);
	configManager->SetVariable("enemy", "hitColorMaxTime", &damageCoolMaxTime_);

	// クールタイム
    configManager->SetVariable("enemy", "damageCoolTimeMax", &damageCoolMaxTime_);

#pragma endregion // ConfigManager

	rootMove_.MaxNumMovePhase = rand() % rootMove_.MaxRandMovePhase + 1;
	rootMove_.isBehavior_ = false;
	rootMove_.startPos = worldTransform_.transform_;
	rootMove_.targetPos.x = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	rootMove_.targetPos.z = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	rootMove_.targetPos.y = worldTransform_.transform_.y;
	rootMove_.transitionFactor = 0;
	rootMove_.numMovePhase = 0;
	rootMove_.coolTime = 0;
	if (rootMove_.targetPos.x >= 0) {
		rootMove_.targetPos.x += rootMove_.MinMove;
	}
	else {
		rootMove_.targetPos.x -= rootMove_.MinMove;
	}
	if (rootMove_.targetPos.z >= 0) {
		rootMove_.targetPos.z += rootMove_.MinMove;
	}
	else {
		rootMove_.targetPos.z -= rootMove_.MinMove;
	}

	// 対象から対象へのベクトル
	Vector3 sub = Subtract(rootMove_.targetPos, rootMove_.startPos);

	float length = Length(sub);

	// Y軸周り角度
	worldPrediction_.transform_ = rootMove_.startPos;
	worldPrediction_.transform_.y = -2.5f;
	worldPrediction_.rotate_.y = std::atan2(sub.x, sub.z);
	worldPrediction_.scale_ = { 1 * 5 ,1 * 5,length / 2 };

	//音
	audio_ = std::make_unique<Audio>();
	audio_->Initialize();

	sound = audio_->SoundLoadWave("resources/Sounds/Alarm01.wav");

	// 移動音
	///sounds_.playerMove = audio_->SoundLoadWave("resources/Sounds/playerMove.wav");





	hp = MaxHp;
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

			// 音入れ
			//audio_->SoundPlay(sound, 0.2f,false);

			if (attackBehaviorOld_ == AttackBehavior::kSpecial) {
				if (specialAttackBehaviorOld_ == SpecialAttack::kAttack2) {
					follow_->SetT(0);
				}
			};

			break;
		case Behavior::kFear:
			BehaviorFearInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kDie:
			BehaviorDieInitialize();
			//audio_->SoundStop(sound);
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot: // 通常行動更新
		BehaviorRootUpdate();

		follow_->SetFlag(false);
		break;
	case Behavior::kFear: // 怯み行動更新
		BehaviorFearUpdate();
		follow_->SetFlag(false);
		break;
	case Behavior::kAttack: // 攻撃行動更新
		BehaviorAttackUpdate();
		break;
	case Behavior::kDie:
		BehaviorDieUpdate();
		break;
	}



	//
	BulletUpdate();

	// HP
	if (hp <= 0) {
		isAlive = false;
	}
#ifdef _DEBUG


	if (ImGui::BeginTabBar("GameScene"))
	{
		if (ImGui::BeginTabItem("enemy"))
		{

			ImGui::DragInt("hp", &hp, 1.0f);
			int mma = int(MaxHp);
			ImGui::DragInt("MaxHp", &mma, 1.0f);
			MaxHp = uint32_t(mma);
			ImGui::Checkbox("debugAttack", &isDebugAttack);
			if (ImGui::Button("Fear")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kFear;
			}
			if (ImGui::Button("Death")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kDie;
			}
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
			if (ImGui::Button("NormalShotAttack1")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kNormal;
				normalAttackBehaviorRequest_ = NormalAttack::kAttackShort1;
			}
			if (ImGui::Button("NormalShotAttack2")) {
				behaviorTimer_ = 0;
				behaviorRequest_ = Behavior::kAttack;
				attackBehaviorRequest_ = AttackBehavior::kNormal;
				normalAttackBehaviorRequest_ = NormalAttack::kAttackShort2;
			}
			if (ImGui::TreeNode("other"))
			{
				ImGui::ColorEdit4("defaultColor", &defaultColor_.x);
				ImGui::ColorEdit4("hitColor", &hitColor_.x);
				ImGui::DragFloat("hitColorMaxTime", &damageCoolMaxTime_, 0.01f);
                ImGui::DragFloat("damageCoolTimeMax", &damageCoolMaxTime_, 0.01f);

				ImGui::TreePop();
			}
			ImGui::EndTabItem();
			if (ImGui::Button("save"))
			{
				ConfigManager* config = ConfigManager::GetInstance();
				config->SaveData("enemy");
                config->SaveData(" attack");
                config->SaveData("attack1");
                config->SaveData("attack2");
                config->SaveData("attack3");
                config->SaveData("attack4");
                config->SaveData("normalAttackShot1_");
                config->SaveData("normalAttackShot2_");
                config->SaveData("root");
			}
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

		if (ImGui::BeginTabItem("root"))
		{
			ImGui::DragFloat("MaxCoolTime", &rootMove_.MaxCoolTime, 1.0f);
			int move = rootMove_.MaxMove;
			ImGui::DragInt("MaxMove.", &move, 1.0f);
			rootMove_.MaxMove = move;
			ImGui::DragFloat("MaxRandCoolTime", &rootMove_.MaxRandCoolTime, 0.01f);
			ImGui::DragFloat3("worldTransformRight_.transform_", &worldTransformRight_.transform_.x, 0.01f);
			int phase = rootMove_.MaxRandMovePhase;
			ImGui::DragInt3("MaxRandMovePhase", &phase, 0.01f);
			rootMove_.MaxRandMovePhase = phase;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("attack"))
		{
			int phase1 = atMethod_.probabilityPhase1;
			ImGui::DragInt("probabilityPhase1", &phase1, 1.0f);
			atMethod_.probabilityPhase1 = phase1;
			int phase2 = atMethod_.probabilityPhase2;
			ImGui::DragInt("probabilityPhase2", &phase2, 1.0f);
			atMethod_.probabilityPhase2 = phase2;
			int phase3 = atMethod_.probabilityPhase3;
			ImGui::DragInt("probabilityPhase3", &phase3, 1.0f);
			atMethod_.probabilityPhase3 = phase3;
			ImGui::DragFloat("distanceSwich", &atMethod_.distanceSwich, 0.1f);
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
			ImGui::DragFloat("ExpandTime", &attack2_.MaxExpandTime, 0.01f);
			ImGui::DragFloat("attackPower", &attack2_.attackPower, 0.01f);
			ImGui::DragFloat("poreparationTime", &attack2_.MaxAttackPreparationTime, 0.01f);
			ImGui::DragFloat("maxSize", &attack2_.maxSize, 0.01f);
			ImGui::DragFloat("minSize", &attack2_.minSize, 0.01f);
			ImGui::DragFloat("MaxDeathTimer_", &attack2_.MaxDeathTimer_, 0.01f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("attack3"))
		{
			ImGui::DragFloat("cooldown", &attack3_.MaxAttackCooldown, 0.01f);
			ImGui::DragFloat("MinPosY", &attack3_.MinPosY, 0.01f);
			ImGui::DragFloat("MaxPosY", &attack3_.MaxPosY, 0.01f);
			ImGui::DragFloat("MinYTime", &attack3_.MinYTime, 0.01f);
			ImGui::DragFloat("MaxYTime", &attack3_.MaxYTime, 0.01f);
			ImGui::DragInt("numShotsPerPhase", &attack3_.MaxNumShotsPerPhase, 1.0f);
			ImGui::DragFloat("attackPower", &attack3_.attackPower, 0.01f);
			ImGui::DragFloat("AttackCooldown", &attack3_.MaxAttackCooldown, 0.01f);
			int ii = int(attack3_.numElectricCount);
			ImGui::DragInt("numElectricCount", &ii, 1.0f);
			attack3_.numElectricCount = uint32_t(ii);
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
			//ImGui::DragFloat("StoppingTime", &attack4_.MaxStoppingTime, 0.01f);
			ImGui::DragFloat("speed", &attack4_.speed, 0.01f);
			ImGui::DragFloat("attackPower", &attack4_.attackPower, 0.01f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("normalAttackShot1_"))
		{
			ImGui::DragFloat("ArmGrowthToSpinDelay", &normalAttackShot1_.MaxArmGrowthToSpinDelay, 0.01f);
			ImGui::DragFloat("cooldown", &normalAttackShot1_.cooldownTime, 0.01f);
			ImGui::DragFloat("SpinTime", &normalAttackShot1_.MaxSpinTime, 0.01f);
			ImGui::DragFloat("MaxRotateSpeed", &normalAttackShot1_.MaxRotateSpeed, 0.01f);
			ImGui::DragFloat("MinRotateSpeed", &normalAttackShot1_.MinxRotateSpeed, 0.01f);
			//ImGui::DragFloat("StoppingTime", &normalAttackShot1_.MaxStoppingTime, 0.01f);
			ImGui::DragFloat("speed", &normalAttackShot1_.speed, 0.01f);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("normalAttackShot2_"))
		{
			//normalAttackShot2_.
			ImGui::DragFloat("ArmGrowthToSpinDelay", &normalAttackShot2_.MaxArmGrowthToSpinDelay, 0.01f);
			ImGui::DragFloat("cooldown", &normalAttackShot2_.cooldownTime, 0.01f);
			ImGui::DragFloat("MaxAssaultTime", &normalAttackShot2_.MaxAssaultTime, 0.01f);
			ImGui::DragFloat("StoppingTime", &normalAttackShot2_.MaxStoppingTime, 0.01f);
			ImGui::DragFloat("speed", &normalAttackShot2_.speed, 0.01f);
			ImGui::DragFloat("rotateSpeed", &normalAttackShot2_.transitionFactorSpeed, 0.01f);
			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}
#endif // _DEBUG

	StageMovementRestrictions();

	// エミッターの更新
	if (die_.enmey) {
		UpdateParticleEmitter();
	}
	// ワールドトランスフォーム更新
	worldTransform_.UpdateData();
	worldTransformLeft_.UpdateData();
	worldTransformRight_.UpdateData();
	worldTransformBody_.UpdateData();

	worldPrediction_.UpdateData();


	if (isAlive)
	{
		UpdateHitColor();
        bodyCollider_->RegsterCollider();
	}



}

void Enemy::Draw(const Camera& camera)
{
	if (die_.enmey) {
		model_->Draw(worldTransformBody_, &camera, &color_);
	}
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
		if (rootMove_.numMovePhase < rootMove_.MaxNumMovePhase) {
			modelPrediction_->Draw(worldPrediction_, &camera, &colorPrediction_);
		}
		break;
	case Behavior::kDie:

		deashExplosionParticle_->Draw();

		if (die_.enmey) {
			for (uint32_t index = 0; index < deashSmokeParticle_.size(); ++index)
			{
				deashSmokeParticle_[index]->Draw();
			}
		}
		break;
	case Behavior::kAttack:

		// 攻撃遷移
		switch (attackBehavior_) {
		case AttackBehavior::kNormal: // 通常攻撃の場合
			switch (normalAttackBehavior_)
			{
			case Enemy::NormalAttack::kAttackShort1: // 近距離1
				modelLeftArm_->Draw(worldTransformLeft_, &camera, &color_);
				modelRightArm_->Draw(worldTransformRight_, &camera, &color_);
				break;
			case Enemy::NormalAttack::kAttackShort2: // 近距離2
				modelLeftArm_->Draw(worldTransformLeft_, &camera, &color_);
				modelRightArm_->Draw(worldTransformRight_, &camera, &color_);
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



#ifdef _DEBUG
	bodyCollider_->Draw();
#endif // _DEBUG


}

void Enemy::OnCollision([[maybe_unused]] const Collider* _other)
{
	if (_other->GetName() == "weapon")
	{
		if(!isCoolTime_)
		{
			color_.SetColor(hitColor_);
			isHitColor_ = true;
			isCoolTime_ = true;

			hp  -= player_->GetDamege();
            if (hp <= 0)
            {
				if (isAlive) {
					behaviorRequest_ = Behavior::kDie;
				}

                isAlive = false;
            }
		}
	}
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

bool Enemy::IsStageMovementRestrictions()
{
	if (stage_->GetWallBack().z < worldTransform_.GetWorldPosition().z) {
		return true;
	}
	else if (stage_->GetWallFlont().z > worldTransform_.GetWorldPosition().z) {
		return true;
	}
	if (stage_->GetWallLeft().x > worldTransform_.GetWorldPosition().x) {
		return true;
	}
	else if (stage_->GetWallRight().x < worldTransform_.GetWorldPosition().x) {
		return true;
	}
	return false;
}


#pragma region 状態遷移

#pragma region Root

void Enemy::BehaviorRootInitialize()
{
	// 浮遊ギミック
	InitializeFloatingGimmick();

	// 不規則な移動数
	rootMove_.MaxNumMovePhase = rand() % rootMove_.MaxRandMovePhase + 1;
	rootMove_.isBehavior_ = false;
	rootMove_.startPos = worldTransform_.transform_;
	rootMove_.targetPos.x = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	rootMove_.targetPos.z = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
	if (rootMove_.targetPos.x >= 0) {
		rootMove_.targetPos.x += rootMove_.MinMove;
	}
	else {
		rootMove_.targetPos.x -= rootMove_.MinMove;
	}
	if (rootMove_.targetPos.z >= 0) {
		rootMove_.targetPos.z += rootMove_.MinMove;
	}
	else {
		rootMove_.targetPos.z -= rootMove_.MinMove;
	}

	rootMove_.targetPos.y = worldTransform_.transform_.y;
	rootMove_.transitionFactor = 0;
	rootMove_.numMovePhase = 0;
	rootMove_.coolTime = 0;


	// 対象から対象へのベクトル
	Vector3 sub = Subtract(rootMove_.targetPos, rootMove_.startPos);

	float length = Length(sub);

	// Y軸周り角度
	worldPrediction_.transform_ = rootMove_.startPos;
	worldPrediction_.transform_.y = -2.5f;
	worldPrediction_.rotate_.y = std::atan2(sub.x, sub.z);
	worldPrediction_.scale_ = { 1 * 5 ,1 * 5,length / 2 };
}

void Enemy::BehaviorRootUpdate()
{
	//float transitionSpeed = 0.01f; // 補間速度（0.0f〜1.0fの間）

	// 行動遷移しない
	if (rootMove_.isBehavior_ == false) {
		// フェーズを繰り返す
		if (rootMove_.numMovePhase < rootMove_.MaxNumMovePhase) {
			// 移動
			if (++rootMove_.startDelay > rootMove_.MaxStartDelay) {



				rootMove_.transitionFactor += rootMove_.transitionSpeed;
			}

			if (rootMove_.transitionFactor >= 1.0f) {

				if (++rootMove_.coolTime > rootMove_.MaxCoolTime) {
					// 0に

					// クールタイムを0に
					rootMove_.coolTime = 0;
					// 始点を現在の位置に
					rootMove_.startPos = worldTransform_.transform_;
					worldPrediction_.transform_ = worldTransform_.transform_;

					// 終点をランダムに(一旦)
					rootMove_.targetPos.x = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
					rootMove_.targetPos.z = float(rand() % rootMove_.MaxMove - ((rootMove_.MaxMove / 2)));
					rootMove_.targetPos.y = worldTransform_.transform_.y;
					rootMove_.MaxCoolTime = float(rand() % int(rootMove_.MaxRandCoolTime) + 10);

					rootMove_.transitionSpeed = float(float(float(rand() % 100 + 100) / 10000.0f));




					// 対象から対象へのベクトル
					Vector3 sub = Subtract(rootMove_.targetPos, rootMove_.startPos);

					float length = Length(sub);

					// Y軸周り角度
					worldPrediction_.rotate_.y = std::atan2(sub.x, sub.z);
					worldPrediction_.scale_ = { 1 * 5 ,1 * 5,length / 2 };
					worldPrediction_.transform_.y = -2.5f;

					//フェーズを加算
					rootMove_.numMovePhase++;
					rootMove_.transitionFactor = 0.0f;

					rootMove_.startDelay = 0.0f;
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

void Enemy::Move(float _speed, bool flag)
{
	if (DistanceXZ(player_->GetWorldTransform().GetWorldPosition(), worldTransform_.GetWorldPosition()) >= 1) {
		// 回転と移動量の設定
		const float kMoveSpeed = _speed; // 移動速度
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

		normalAttackShot2_.moveDirection = moveDirection;

		worldTransform_.transform_ += moveDirection;
	}
}

#pragma endregion // 通常行動

#pragma region Attack

void Enemy::BehaviorAttackInitialize()
{


	if (!isDebugAttack) {
		//atMethod_.randMethod = rand() % sizeof(allAttack_);
		if (hp > MaxHp / 2) { // HPが最大HPの1/2より大きい場合
			atMethod_.normalProbability = atMethod_.probabilityPhase1;
		}
		else if (hp > MaxHp / 3) { // HPが最大HPの1/3より大きい場合
			atMethod_.normalProbability = atMethod_.probabilityPhase2;
		}
		else { // HPが最大HPの1/3以下の場合
			atMethod_.normalProbability = atMethod_.probabilityPhase3;
		}

		// 確率を出す
		atMethod_.randMethod = rand() % 100;
		// 通常攻撃より高い値が出たら
		if (atMethod_.randMethod >= atMethod_.normalProbability) {

			if (hp > MaxHp / 2) { // HPが最大HPの1/2より大きい場合
				// 必殺攻撃を確率で選ぶ
				atMethod_.randAttack = rand() % 2 + 1;
				if (atMethod_.randAttack == 1) {
					allAttack_ = AllAttack::kSpecialAttack;
				}
				else if (atMethod_.randAttack == 2) {
					allAttack_ = AllAttack::kSpecialAttack3;
				}
			}
			else if (hp > MaxHp / 3) { // HPが最大HPの1/3より大きい場合
				// 必殺攻撃を確率で選ぶ
				atMethod_.randAttack = rand() % 3 + 1;
				if (atMethod_.randAttack == 1) {
					allAttack_ = AllAttack::kSpecialAttack;
				}
				else if (atMethod_.randAttack == 2) {
					allAttack_ = AllAttack::kSpecialAttack3;
				}
				else if (atMethod_.randAttack == 3) {
					allAttack_ = AllAttack::kSpecialAttack4;
				}
			}
			else { // HPが最大HPの1/3より大きい場合
				atMethod_.randAttack = rand() % 4 + 1;
				// 必殺攻撃を確率で選ぶ
				if (atMethod_.randAttack == 1) {
					allAttack_ = AllAttack::kSpecialAttack;
				}
				else if (atMethod_.randAttack == 2) {
					allAttack_ = AllAttack::kSpecialAttack2;
				}
				else if (atMethod_.randAttack == 3) {
					allAttack_ = AllAttack::kSpecialAttack3;
				}
				else if (atMethod_.randAttack == 4) {
					allAttack_ = AllAttack::kSpecialAttack4;
				}
			}

		}
		else {// 通常攻撃より低い値が出たら


			if (atMethod_.distanceSwich >= DistanceXZ(worldTransform_.GetWorldPosition(), player_->GetWorldTransform().GetWorldPosition())) {
				atMethod_.randAttack = rand() % 2 + 1;
				if (atMethod_.randAttack == 1) {
					allAttack_ = AllAttack::kNormalShort1;
				}
				else if (atMethod_.randAttack == 2) {
					allAttack_ = AllAttack::kNormalShort2;
				}
			}
			else {
				atMethod_.randAttack = rand() % 2 + 1;
				if (atMethod_.randAttack == 1) {
					allAttack_ = AllAttack::kNormalLong1;
				}
				else if (atMethod_.randAttack == 2) {
					allAttack_ = AllAttack::kNormalLong2;
				}
			}
		}
	}
	else {
		allAttack_ = AllAttack::kNone;
	}

	switch (allAttack_)
	{
	case Enemy::AllAttack::kNormalShort1:
		attackBehaviorRequest_ = AttackBehavior::kNormal;
		normalAttackBehaviorRequest_ = NormalAttack::kAttackShort1;
		break;
	case Enemy::AllAttack::kNormalShort2:
		attackBehaviorRequest_ = AttackBehavior::kNormal;
		normalAttackBehaviorRequest_ = NormalAttack::kAttackShort2;
		break;
	case Enemy::AllAttack::kNormalLong1:
		attackBehaviorRequest_ = AttackBehavior::kNormal;
		normalAttackBehaviorRequest_ = NormalAttack::kAttackLong1;
		break;
	case Enemy::AllAttack::kNormalLong2:
		attackBehaviorRequest_ = AttackBehavior::kNormal;
		normalAttackBehaviorRequest_ = NormalAttack::kAttackLong2;
		break;
	case Enemy::AllAttack::kSpecialAttack:
		attackBehaviorRequest_ = AttackBehavior::kSpecial;
		specialAttackBehaviorRequest_ = SpecialAttack::kAttack;
		break;
	case Enemy::AllAttack::kSpecialAttack2:
		attackBehaviorRequest_ = AttackBehavior::kSpecial;
		specialAttackBehaviorRequest_ = SpecialAttack::kAttack2;
		break;
	case Enemy::AllAttack::kSpecialAttack3:
		attackBehaviorRequest_ = AttackBehavior::kSpecial;
		specialAttackBehaviorRequest_ = SpecialAttack::kAttack3;
		break;
	case Enemy::AllAttack::kSpecialAttack4:
		attackBehaviorRequest_ = AttackBehavior::kSpecial;
		specialAttackBehaviorRequest_ = SpecialAttack::kAttack4;
		break;
	default:
		break;
	}

	if (attackBehaviorRequest_) {
		// ふるまいを変更する
		attackBehavior_ = attackBehaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (attackBehavior_) {
		case AttackBehavior::kNormal: // 通常攻撃の場合

			attackBehaviorOld_ = AttackBehavior::kNormal;

			if (normalAttackBehaviorRequest_) {
				// ふるまい
				normalAttackBehavior_ = normalAttackBehaviorRequest_.value();
				switch (normalAttackBehavior_)
				{
				case Enemy::NormalAttack::kAttackShort1: // 近距離1
					NormalShotAttack1Initialize();
					damage_ = 5;
					break;
				case Enemy::NormalAttack::kAttackShort2: // 近距離2
					NormalShotAttack2Initialize();
					damage_ = 5;
					break;
				case Enemy::NormalAttack::kAttackLong1:  // 遠距離1
					NormalLongAttack1Initialize();
					damage_ = 2;
					break;
				case Enemy::NormalAttack::kAttackLong2:  // 遠距離2
					NormalLongAttack2Initialize();
					damage_ = 2;
					break;
				default:
					break;
				}
				normalAttackBehaviorRequest_ = std::nullopt;
			}
			break;
		case AttackBehavior::kSpecial: // 必殺技攻撃の場合

			attackBehaviorOld_ = AttackBehavior::kSpecial;

			if (specialAttackBehaviorRequest_) {
				specialAttackBehavior_ = specialAttackBehaviorRequest_.value();

				switch (specialAttackBehavior_)
				{
				case Enemy::SpecialAttack::kAttack:		// 攻撃1
					SpecialAttackInitialize();
					specialAttackBehaviorOld_ = Enemy::SpecialAttack::kAttack;
					damage_ = attack1_.attackPower;
					break;
				case Enemy::SpecialAttack::kAttack2:	// 攻撃2
					SpecialAttack2Initialize();
					specialAttackBehaviorOld_ = Enemy::SpecialAttack::kAttack2;
					follow_->SetT(0);
					damage_ = attack2_.attackPower;
					break;
				case Enemy::SpecialAttack::kAttack3:	// 攻撃3
					specialAttackBehaviorOld_ = Enemy::SpecialAttack::kAttack3;
					SpecialAttack3Initialize();
					damage_ = attack3_.attackPower;
					break;
				case Enemy::SpecialAttack::kAttack4:	// 攻撃4
					specialAttackBehaviorOld_ = Enemy::SpecialAttack::kAttack4;
					SpecialAttack4Initialize();
					damage_ = attack4_.attackPower;
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
	fear_.startPos = worldTransform_.transform_;
	fear_.targetPos = worldTransform_.transform_;
	fear_.targetPos.y = fear_.MinY;
	fear_.transitionFactor = 0;
	fear_.isGetUp = false;
	fear_.timer = 0;
	fear_.getupTimer = 0;

	fear_.rotate = Vector3{ DegreesToRadians(30),worldTransform_.rotate_.y,DegreesToRadians(60) };

	fear_.startRotate = worldTransform_.rotate_;
	fear_.tragetRotate = fear_.rotate;
}

void Enemy::BehaviorFearUpdate()
{


	fear_.transitionFactor += fear_.transitionSpeed;

	if (!fear_.isGetUp) {
		if (fear_.transitionFactor >= 1.0f) {
			fear_.transitionFactor = 1.0f;
			if (++fear_.timer >= fear_.MaxTimer) {
				fear_.transitionFactor = 0.0f;
				fear_.isGetUp = true;

				fear_.startPos = worldTransform_.transform_;
				fear_.targetPos = worldTransform_.transform_;
				fear_.targetPos.y = fear_.oldPosY;

				fear_.startRotate = fear_.rotate;
				fear_.tragetRotate = Vector3{ 0,worldTransform_.rotate_.y,0 };

			}
		}
		worldTransform_.transform_ = Lerp(fear_.startPos, fear_.targetPos, fear_.transitionFactor);
		worldTransform_.rotate_.x = ShortestAngleLerp(fear_.startRotate.x, fear_.tragetRotate.z, fear_.transitionFactor);
		//worldTransform_.rotate_.y = ShortestAngleLerp(fear_.startRotate.y, fear_.tragetRotate.y, fear_.transitionFactor);
		worldTransform_.rotate_.z = ShortestAngleLerp(fear_.startRotate.z, fear_.tragetRotate.z, fear_.transitionFactor);
	}
	if (fear_.isGetUp) {
		if (fear_.transitionFactor >= 1.0f) {
			fear_.transitionFactor = 1.0f;
			if (++fear_.getupTimer >= fear_.MaxGetupTimer) {
				behaviorRequest_ = Behavior::kRoot;
			}
		}

		worldTransform_.transform_ = Lerp(fear_.startPos, fear_.targetPos, fear_.transitionFactor);
		worldTransform_.rotate_.x = ShortestAngleLerp(fear_.startRotate.x, fear_.tragetRotate.z, fear_.transitionFactor);
		//worldTransform_.rotate_.y = ShortestAngleLerp(fear_.startRotate.y, fear_.tragetRotate.y, fear_.transitionFactor);
		worldTransform_.rotate_.z = ShortestAngleLerp(fear_.startRotate.z, fear_.tragetRotate.z, fear_.transitionFactor);

	}
}

#pragma endregion // 怯み行動

#pragma region Die

void Enemy::BehaviorDieInitialize()
{
	die_.coolTime = 0;
	die_.shakeTime = 0;
	die_.isExplosion = false;
	die_.shakePos = worldTransform_.transform_;
	for (int i = 0; i < 5; i++) {
		die_.smokeFlag[i] = false;
	}
}

void Enemy::BehaviorDieUpdate()
{



	if (die_.coolTime >= die_.MaxCoolTime) {

		die_.isExplosion = false;
		die_.enmey = false;
	}

	// カウントを5回まで
	if (die_.smokeCount < 5) {
		// 煙を続々出していく
		if (++die_.smokeTimer >= die_.MaxSmokeTimer) {


			die_.smokeFlag[die_.smokeCount] = true;

			die_.smokeTimer = 0;
			die_.smokeCount++;
		}
	}
	else {
		// シェイク
		if (++die_.shakeTime <= die_.MaxShakeTime) {
			Vector3 shake = Vector3(rand() % 5 - 2, rand() % 3 - 1, rand() % 3 - 1);
			worldTransform_.transform_ = die_.shakePos + shake;


		}
		else {
			Vector3 shake = Vector3(rand() % 5 - 2, rand() % 3 - 1, rand() % 3 - 1);
			worldTransform_.transform_ = die_.shakePos + shake;

			if (die_.enmey) {
				die_.isExplosion = true;
			}
		}
	}

	if (die_.isExplosion) {
		die_.coolTime++;

	}



	// 爆発パーティクル
	deashExplosionParticle_->Update(die_.isExplosion);



	// 煙パーティクル
	if (die_.enmey) {
		for (uint32_t index = 0; index < deashSmokeParticle_.size(); ++index)
		{
			deashSmokeParticle_[index]->Update(die_.smokeFlag[index]);
		}
	}

}

#pragma endregion // 死亡

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



	std::random_device rd; // ランダムデバイス
	std::mt19937 gen(rd()); // メルセンヌ・ツイスタ
	std::uniform_int_distribution<> dist(0, 1); // 0または1の範囲

	attack1_.randAttack = dist(gen); // ランダム値を生成

	if (attack1_.randAttack == 0) {
		attack1_.availableLocations = { 0, 1, 2, 3, 4 };

		// 乱数生成器
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::shuffle(attack1_.availableLocations.begin(), attack1_.availableLocations.end(), gen);

		if (hp <= MaxHp) {
			if (hp >= MaxHp / 2) {
				attack1_.MaxArmNum = 3;
			}
			else {
				attack1_.MaxArmNum = 5;
			}
		}
	}
	else {
		std::random_device rd; // ランダムデバイス
		std::mt19937 gen(rd()); // メルセンヌ・ツイスタ
		std::uniform_int_distribution<> dist(0, 1); // 0または1の範囲

		attack1_.rrr = dist(gen); // ランダム値を生成

		attack1_.MaxArmNum = 4;
	}
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
			// 初期化処理
			if (attack1_.armNum < attack1_.MaxArmNum) {
				if (attack1_.randAttack == 0) {
					// 次の場所をリストから取得
					int newLocation = attack1_.availableLocations[attack1_.armNum];

					// 初期化処理
					StageArmInitialize(newLocation);
				}
				else {


					if (attack1_.rrr == 0) {
						StageArmInitialize(Stage::StageNum::kLeft, (attack1_.armNum - 2));
						StageArmInitialize(Stage::StageNum::kRight, (attack1_.armNum - 2));
					}
					else {
						StageArmInitialize(Stage::StageNum::kFlont, (attack1_.armNum - 2));
						StageArmInitialize(Stage::StageNum::kBack, (attack1_.armNum - 2));
					}



				}
				// 状態の更新
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
	else if (num == Stage::StageNum::kFlont) {
		stagePos.x = player_->GetWorldTransform().GetWorldPosition().x;
		stagePos.y = player_->GetWorldTransform().GetWorldPosition().y;
		stagePos.z = stage_->GetWallFlont().z - 20;
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

void Enemy::StageArmInitialize(int num, int i)
{
	const float kBulletSpeed = attack1_.armSpeed;
	Vector3 velocityB{};

	//
	Vector3 direction{};
	Vector3 stagePos{};
	//direction = player_->GetWorldTransform().GetWorldPosition();
	direction.x =  ((i + 1) * 25) - 15;
	direction.z =  ((i + 1) * 25) - 15;

	// 床から
	if (num == Stage::StageNum::kFloor) {
		stagePos.x = direction.x;
		stagePos.y = stage_->GetWallFloor().y - 20;
		stagePos.z = direction.z;
	}
	else if (num == Stage::StageNum::kFlont) {
		stagePos.x = direction.x;
		stagePos.y = direction.y;
		stagePos.z = stage_->GetWallFlont().z - 20;
	}
	else if (num == Stage::StageNum::kBack) {
		stagePos.x = direction.x;
		stagePos.y = direction.y;
		stagePos.z = stage_->GetWallBack().z + 20;
	}
	else if (num == Stage::StageNum::kLeft) {
		stagePos.x = stage_->GetWallLeft().x - 20;
		stagePos.y = direction.y;
		stagePos.z = direction.z;
	}
	else if (num == Stage::StageNum::kRight) {
		stagePos.x = stage_->GetWallRight().x + 20;
		stagePos.y = direction.y;
		stagePos.z = direction.z;
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

void Enemy::ThunderInitialize([[maybe_unused]] Vector3 pos)
{
	follow_->SetFlag(true);


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

		//float rotate = float(attack3_.numShotsPerPhase / 3000);

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
	//attack4_.stoppingTime = 0;
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


	leftArmCollider_->RegsterCollider();
	rightArmCollider_->RegsterCollider();

#ifdef _DEBUG
	leftArmCollider_->Draw();
	rightArmCollider_->Draw();
#endif // _DEBUG

}

#pragma endregion // 攻撃4

#pragma endregion //必殺技


#pragma region Noraml

#pragma region NormalAttackShot1

void Enemy::NormalShotAttack1Initialize()
{
	normalAttackShot1_.transitionFactor = 0;
	worldTransformLeft_.transform_ = { 0,0,0 };
	worldTransformRight_.transform_ = { 0,0,0 };
	normalAttackShot1_.spinTime = 0;
	normalAttackShot1_.armGrowthToSpinDelay = 0;
	//normalAttackShot1_.stoppingTime = 0;
	normalAttackShot1_.recoilTime = 0;
}

void Enemy::NormalShotAttack1Update()
{
	static float transitionFactor = 0.01f;

	normalAttackShot1_.transitionFactor += transitionFactor;

	float str = 0;
	float end = 4;
	float endm = -4;
	if (++normalAttackShot1_.armGrowthToSpinDelay <= normalAttackShot1_.MaxArmGrowthToSpinDelay) {
		worldTransformLeft_.transform_.x = StartEnd(str, endm, normalAttackShot1_.transitionFactor);
		worldTransformRight_.transform_.x = StartEnd(str, end, normalAttackShot1_.transitionFactor);
		if (worldTransformLeft_.transform_.x >= endm) {
			worldTransformBody_.rotate_.y -= 0.01f;
		}
	}
	else {

		if (++normalAttackShot1_.spinTime <= normalAttackShot1_.MaxSpinTime) {
			Move(normalAttackShot1_.speed, true);

			normalAttackShot1_.rotateT += transitionFactor;
			if (normalAttackShot1_.rotateT >= 1.0f) {
				normalAttackShot1_.rotateT = 1;
			}
			normalAttackShot1_.rotateSpeed = Lerp(normalAttackShot1_.MinxRotateSpeed, normalAttackShot1_.MaxRotateSpeed, normalAttackShot1_.rotateT);


			worldTransformBody_.rotate_.y += normalAttackShot1_.rotateSpeed;


		}
		else {
			normalAttackShot1_.rotateT -= transitionFactor;
			if (normalAttackShot1_.rotateT <= 0.0f) {
				normalAttackShot1_.rotateT = 0.0f;
			}

			normalAttackShot1_.rotateSpeed = Lerp(normalAttackShot1_.MinxRotateSpeed, normalAttackShot1_.MaxRotateSpeed, normalAttackShot1_.rotateT);
			if (normalAttackShot1_.rotateSpeed <= 0.011f) {
				normalAttackShot1_.rotateSpeed = 0;

			}

			worldTransformBody_.rotate_.y += normalAttackShot1_.rotateSpeed;




			if (normalAttackShot1_.rotateT <= 0) {
				behaviorTimer_++;
				if (behaviorTimer_ >= normalAttackShot1_.cooldownTime) {
					behaviorRequest_ = Behavior::kRoot;
					behaviorTimer_ = 0;
				}
			}
		}
	}
	leftArmCollider_->RegsterCollider();
	rightArmCollider_->RegsterCollider();

#ifdef _DEBUG
	leftArmCollider_->Draw();
	rightArmCollider_->Draw();
#endif // _DEBUG
}

#pragma endregion // 通常近距離攻撃1

#pragma region NormalAttackShot2

void Enemy::NormalShotAttack2Initialize()
{
	normalAttackShot2_.transitionFactor = 0;
	worldTransformLeft_.transform_ = { 0,0,0 };
	worldTransformRight_.transform_ = { 0,0,0 };
	normalAttackShot2_.assaultTime = 0;
	normalAttackShot2_.armGrowthToSpinDelay = 0;
	normalAttackShot2_.stoppingTime = 0;
	normalAttackShot2_.recoilTime = 0;

	Move(normalAttackShot2_.speed, false);
}

void Enemy::NormalShotAttack2Update()
{

	normalAttackShot2_.transitionFactor += normalAttackShot2_.transitionFactorSpeed;

	float str = 0;
	float end = 4;
	float endm = -4;

	float rotS = DegreesToRadians(0);
	float rotE = DegreesToRadians(90);
	float rotEm = DegreesToRadians(-90);
	if (++normalAttackShot2_.armGrowthToSpinDelay <= normalAttackShot2_.MaxArmGrowthToSpinDelay || normalAttackShot2_.transitionFactor <= 1.0f) {
		if (normalAttackShot2_.transitionFactor >= 1.0f) {
			normalAttackShot2_.transitionFactor = 1.0f;
		}
		worldTransformLeft_.transform_.x = StartEnd(str, endm, normalAttackShot2_.transitionFactor);
		worldTransformRight_.transform_.x = StartEnd(str, end, normalAttackShot2_.transitionFactor);

		worldTransformLeft_.rotate_.y = StartEnd(rotS, rotE, normalAttackShot2_.transitionFactor);
		worldTransformRight_.rotate_.y = StartEnd(rotS, rotEm, normalAttackShot2_.transitionFactor);

		if (worldTransformLeft_.transform_.x <= endm) {

			worldTransformLeft_.rotate_.y = DegreesToRadians(90);
			worldTransformRight_.rotate_.y = DegreesToRadians(-90);
		}
	}
	else {

		if (++normalAttackShot2_.assaultTime <= normalAttackShot2_.MaxAssaultTime) {

			if (IsStageMovementRestrictions()) {
				normalAttackShot2_.assaultTime = normalAttackShot2_.MaxAssaultTime;
				normalAttackShot2_.cooldownTime = 120;
			}
			else {
				worldTransform_.transform_ += normalAttackShot2_.moveDirection * 5.0f;
			}

		}
		else {




			behaviorTimer_++;
			if (behaviorTimer_ >= normalAttackShot2_.cooldownTime) {
				behaviorRequest_ = Behavior::kRoot;
				worldTransformLeft_.rotate_.y = 0;
				worldTransformRight_.rotate_.y = 0;
				behaviorTimer_ = 0;
			}

		}

	}
	leftArmCollider_->RegsterCollider();
	rightArmCollider_->RegsterCollider();

#ifdef _DEBUG
	leftArmCollider_->Draw();
	rightArmCollider_->Draw();
#endif // _DEBUG
}

#pragma endregion // 通常近距離攻撃２

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

		//float rotate = float(normal2AttackBullet_.numShotsPerPhase / 3000);

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
		damageCoolTimer_ += 1.0f / 60.0f;
		if (damageCoolTimer_ >= damageCoolMaxTime_)
		{
			damageCoolTimer_ = 0;
			isHitColor_ = false;
            isCoolTime_ = false;
			color_.SetColor(defaultColor_);
		}
	}
}


