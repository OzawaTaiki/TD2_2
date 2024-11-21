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

#include "ParticleEmitters.h"

//
#include <list>
#include <optional>

class Player;

class Stage;

class Enemy
{
public: //ふるまい関係
	// 振るまい
	enum class Behavior {
		kRoot,		// 通常状態
		kFear,      // 怯み状態
		kAttack,	// 攻撃選択
	};

	//振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


#pragma region Attack

	// 攻撃選択
	enum class AttackBehavior
	{
		kNormal,	// 通常攻撃
		kSpecial,	// 必殺攻撃
	};

	// 攻撃選択
	AttackBehavior attackBehavior_ = AttackBehavior::kNormal;
	std::optional<AttackBehavior> attackBehaviorRequest_ = std::nullopt;

	// 通常攻撃方法
	enum class NormalAttack {
		kAttackShort1,
		kAttackShort2,
		kAttackLong1,
		kAttackLong2,
	};

	NormalAttack normalAttackBehavior_ = NormalAttack::kAttackShort1;
	std::optional<NormalAttack> normalAttackBehaviorRequest_ = std::nullopt;

	// 必殺技攻撃方法
	enum class SpecialAttack {
		kAttack,	// 攻撃1中
		kAttack2,   // 攻撃2中
		kAttack3,   // 攻撃3中
		kAttack4,   // 攻撃4中
	};

	SpecialAttack specialAttackBehavior_ = SpecialAttack::kAttack;
	std::optional<SpecialAttack> specialAttackBehaviorRequest_ = std::nullopt;

#pragma endregion // 攻撃関係

#pragma region Move
	// 移動
	struct RootMove {

		Vector3 startPos;
		Vector3 targetPos;

		// t補間用
		float transitionFactor = 0;
		// 追ってくる速度
		float speed = 0.3f;
		// 移動時間
		float moveTime;
		float MaxMoveTime = 60;


		// 
		int MaxNumMovePhase = 1;
		int MaxRandMovePhase = 3;
		int numMovePhase = 0;

		// 
		bool isBehavior_ = false;
	};
	RootMove rootMove_;

#pragma endregion // 通常行動関係


	




private: //状態

	/// <summary>
	/// 行動遷移
	/// </summary>

	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	//怯み行動初期化
	void BehaviorFearInitialize();

	//怯み行動更新
	void BehaviorFearUpdate();





	
#pragma region Attack

	/// <summary>
	/// 通常攻撃
	/// </summary>

	// 通常近距離攻撃1
	void NormalShotAttack1Initialize();

	void NormalShotAttack1Update();

	// 通常近距離攻撃2
	void NormalShotAttack2Initialize();

	void NormalShotAttack2Update();

	// 通常近距離攻撃1
	void NormalLongAttack1Initialize();

	void NormalLongAttack1Update();

	// 通常近距離攻撃2
	void NormalLongAttack2Initialize();

	void NormalLongAttack2Update();

	/// <summary>
	/// 必殺技
	/// </summary>

	// 攻撃行動初期化
	void SpecialAttackInitialize();

	// 攻撃行動更新
	void SpecialAttackUpdate();

	// 攻撃行動初期化
	void SpecialAttack2Initialize();

	// 攻撃行動更新
	void SpecialAttack2Update();

	// 攻撃行動初期化
	void SpecialAttack3Initialize();

	// 攻撃行動更新
	void SpecialAttack3Update();

	// 攻撃行動初期化
	void SpecialAttack4Initialize();

	// 攻撃行動更新
	void SpecialAttack4Update();

#pragma endregion // 攻撃行動関係



	// エミッターの初期化
	void InitializeParticleEmitter();

	// エミッターの更新
	void UpdateParticleEmitter();


    // ヒットカラーの更新
    void UpdateHitColor();

public:
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
	
	void NormalBulletInitialize(Vector3 pos);
	
	void Normal2BulletInitialize(Vector3 pos);
	
	


	// 弾更新
	void BulletUpdate();

	// 弾の初期化
	void StageArmInitialize(int num);

	// 雷の初期化
	void ThunderInitialize(Vector3 pos);
	 // 衝突処理
	void OnCollision();

	void StageMovementRestrictions();

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

public:
	void SetPlayer(Player* player) { player_ = player; };
	void SetStage(Stage* stage) { stage_ = stage; }
	const Behavior& GetBehavior() const { return behavior_; };
	const NormalAttack& GetNormalAttack() const { return normalAttackBehavior_; };
	const SpecialAttack& GetSpecialAttack() const { return specialAttackBehavior_; };

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
	Vector4 defaultColor_ = { 1,1,1,1 };

	// ワールドトランスフォーム
	WorldTransform worldTransform_;			//ベース
	WorldTransform worldTransformBody_;		//体
	WorldTransform worldTransformLeft_;		//左手
	WorldTransform worldTransformRight_;	//右手
	//
	Vector3 oldPos_;
	//
	const Camera* camera_ = nullptr;

	Player* player_;
	Stage* stage_;

	// 電気弾
	std::list<std::unique_ptr<EnemyBullet>> bullets_;
	// 電気弾
	std::list<std::unique_ptr<EnemyBullet>> normalbullets_;
	// ステージアーム攻撃
	std::list<std::unique_ptr<EnemyStageArm>> stageArm;
	// 雷攻撃
	std::list<std::unique_ptr<EnemyThunder>> thunder_;

	std::array<ParticleEmitter, 3>particleEmitter_;


	Camera attackCamera_;
	Camera attackCamera2_;
	Camera attackCamera3_;

    std::unique_ptr<Collider> collider_;

private:
	// 速度
	Vector3 velocity_ = {};
	//
	float speed;


	bool  isDebugAttack = false;

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
	EnemyBullet::Bullet  normalAttackBullet_;
	EnemyBullet::Bullet  normal2AttackBullet_;


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

		// 反動時間
		float recoilTime = 0;
		float MaxRecoilTime = 10;

	};
	SpinAttack attack4_;

	// ヒットカラーの設定
    bool isHitColor_ = false;
    float hitColorTimer_ = 0;
    float hitColorMaxTime_ = 2;
    Vector4 hitColor_ = { 1,0,0,1 };
};