#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include<cmath>


//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"
#include "ConfigManager.h"
#include "../Collider/Collider.h"

// application
#include "EnemyBullet.h"
#include "EnemyStageArm.h"
#include "EnemyThunder.h"

// 
#include <list>
#include <optional>

class Player;

class Stage;

class Enemy
{
public:
	// 振るまい
	enum class Behavior {
		kRoot,		// 通常状態
		kFear,      // 怯み状態
		kAttack,	// 攻撃1中
		kAttack2,   // 攻撃2中
		kAttack3,   // 攻撃3中
		kAttack4,   // 攻撃4中
		kAttack5,   // 攻撃5中
		kAttack6,   // 攻撃6中
	};



	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const Camera& camera);


	void Move(float speed, bool flag);

	// 弾の初期化
	void BulletInitialize(Vector3 pos);

	// 弾更新
	void BulletUpdate();

	// 弾の初期化
	void StageArmInitialize(int num);

	// 雷の初期化
	void ThunderInitialize(Vector3 pos);
	 // 衝突処理
	void OnCollision();
	
	void StageMovementRestrictions();

private:
	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

	//怯み行動初期化
	void BehaviorFearInitialize();

	//怯み行動更新
	void BehaviorFearUpdate();


	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	//攻撃行動初期化
	void BehaviorAttack2Initialize();

	//攻撃行動更新
	void BehaviorAttack2Update();

	//攻撃行動初期化
	void BehaviorAttack3Initialize();

	//攻撃行動更新
	void BehaviorAttack3Update();

	//攻撃行動初期化
	void BehaviorAttack4Initialize();

	//攻撃行動更新
	void BehaviorAttack4Update();







public:
	void SetPlayer(Player* player) { player_ = player; };
	void SetStage(Stage* stage) { stage_ = stage; }
	const Behavior& GetBehavior() const { return behavior_; };

	const Camera& GetCamera() { return attackCamera_; };
	const Camera& GetCamera2() { return attackCamera2_; };
	const Camera& GetCamera3() { return attackCamera3_; };

private:
	// モデル
	Model* model_ = nullptr;
	Model* modelLeftArm_ = nullptr;
	Model* modelRightArm_ = nullptr;
	// モデル
	Model* modelBullet_ = nullptr;
	Model* modelStageArm_ = nullptr;
	Model* modelThunder_ = nullptr;



	// モデルカラー
	ObjectColor color_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformLeft_;
	WorldTransform worldTransformRight_;
	//
	Vector3 oldPos_;
	//
	const Camera* camera_ = nullptr;

	Player* player_;
	Stage* stage_;

	std::list<std::unique_ptr<EnemyBullet>> bullets_;

	std::list<std::unique_ptr<EnemyStageArm>> stageArm;

	std::list<std::unique_ptr<EnemyThunder>> thunder_;




	Camera attackCamera_;
	Camera attackCamera2_;
	Camera attackCamera3_;

    std::unique_ptr<Collider> collider_ = nullptr;
  
private:
	// 速度
	Vector3 velocity_ = {};

	float speed;


	//振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	// 浮遊ギミック媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊サイクル<frame>
	int floatingPeriod = 120;


	// hp
	int hp = 100;
	// 生死フラグ
	bool isAlive = true;
	// 行動タイマー
	int behaviorTimer_;

	// 手の攻撃
	EnemyStageArm::AttackArm attack1_;
	
	// 規則正しい雷
	EnemyThunder::AttackThunder attack2_;

	// 弾
	EnemyBullet::Bullet  attack3_;


	// 攻撃4
	struct SpinAttack {
		// t補間用
		float transitionFactor = 0;
		//腕が生えてから回転開始までの時間）
		float armGrowthToSpinDelay = 0;
		float MaxArmGrowthToSpinDelay = 30.0f;
		// 追ってくる速度
		float speed = 0.3f;

		float rotateT = 0;
		// 回転速度
		float rotateSpeed = 0.1f;
		// Max回転速度
		float MaxRotateSpeed = 1.0f;
		// Mim回転速度
		float MinxRotateSpeed = 0.01f;
		// ぐるぐるし続ける時間
		float spinTime = 0;
		float MaxSpinTime = 120.0f;
		// 動きが止まるまでの時間
		float stoppingTime = 0;
		float MaxStoppingTime = 20;

		// 動きが止まってから回復する（浮く）までの時間
		float cooldownTime = 120;
	};
	SpinAttack attack4_;








};