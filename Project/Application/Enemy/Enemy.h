#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include<cmath>
#include<math.h>

//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"
#include "ConfigManager.h"
#include "../Collider/Collider.h"
#include <MatrixFunction.h>
#include "VectorFunction.h"
#include "Audio.h"

// application
#include "EnemyBullet.h"
#include "EnemyStageArm.h"
#include "EnemyThunder.h"

#include "ParticleEmitters.h"
#include "EnemyDeathParticle.h"

//
#include <list>
#include <optional>

class Player;

class Stage;

class FollowCamera;

class Enemy
{
public: //ふるまい関係
	// 振るまい
	enum class Behavior {
		kRoot,		// 通常状態
		kFear,      // 怯み状態
		kAttack,	// 攻撃選択
		kDie,       // 死亡状態
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
	AttackBehavior attackBehaviorOld_;

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
	SpecialAttack specialAttackBehaviorOld_;

	// 攻撃選択方法
	struct AttackMethod {
		uint32_t randMethod; // 確率で攻撃方法を決める

		uint32_t randAttack;
		// 確率を出すための(変数)
		uint32_t normalProbability; // 通常攻撃の確率

		// 1フェーズ目の通常攻撃確率
		uint32_t probabilityPhase1 = 60;
		// 2フェーズ目の通常攻撃確率
		uint32_t probabilityPhase2 = 25;
		// 3フェーズ目の通常攻撃確率
		uint32_t probabilityPhase3 = 10;

		float distanceSwich = 20;
	};
	AttackMethod atMethod_;

	// 攻撃全般
	enum class AllAttack {
		kNormalShort1,
		kNormalShort2,
		kNormalLong1,
		kNormalLong2,
		kSpecialAttack,	   // 攻撃1中
		kSpecialAttack2,   // 攻撃2中
		kSpecialAttack3,   // 攻撃3中
		kSpecialAttack4,   // 攻撃4中
		kNone,
	};
	AllAttack allAttack_;

#pragma endregion // 攻撃関係

#pragma region Move
	// 移動
	struct RootMove {

		//開始位置
		Vector3 startPos;
		// 終了位置
		Vector3 targetPos;

		// t補間用
		float transitionFactor = 0;
		// t補間スピード
		float transitionSpeed = 0.005f;
		// 追ってくる速度
		float speed = 0.3f;
		// 移動時間(time)
		float moveTime;
		// 移動時間(最大)
		float MaxMoveTime = 60;


		// 移動開始までの時間(time)
		float startDelay = 0;
		// 移動開始までの時間(最大)
		float MaxStartDelay = 40;


		//何回移動するか(最大)
		int MaxNumMovePhase = 1;
		// 移動回数をランダムで出すよう(変数)
		uint32_t MaxRandMovePhase = 2;
		// 何回移動するか(カウント用)
		int numMovePhase = 0;
		// 動き終わった後の止まる時間(time)
		float coolTime = 0;
		// 動き終わった後の止まる時間(最大)
		float MaxCoolTime = 30;
		//  動き終わった後の止まる時間をランダムで不規則にするための(変数)
		float MaxRandCoolTime = 30;
		// 最小値
		float MinMove = 10;

		// 移動距離
		int MaxMove = 30;


		// 状態遷移許可用
		bool isBehavior_ = false;
	};
	RootMove rootMove_;

#pragma endregion // 通常行動関係

#pragma region fear

	// 怯み位置
	struct Fear {

		// t補間スピード
		float transitionSpeed = 0.11f;
		// t補間用
		float transitionFactor = 0;
		//

		// 地面につく
		float MinY = -2;
		float oldPosY = 0;
		// ターゲット位置
		Vector3 targetPos;
		Vector3 tragetRotate;
		Vector3 rotate;

		// スタート位置
		Vector3 startPos;
		Vector3 startRotate;
		// 怯み時間
		uint32_t MaxTimer = 240;
		uint32_t timer = 0;

		bool isGetUp = false;

		uint32_t MaxGetupTimer = 30;
		uint32_t getupTimer = 0;

	};
	Fear fear_;

#pragma endregion // 怯み関係

#pragma region Die

	struct Die {
		bool isFlag = false;

		// カメラワーク時間
		uint32_t cameraWorkTime;
		uint32_t MaxCameraWorkTime = 120;

		// シェイク時間
		uint32_t shakeTime;
		uint32_t MaxShakeTime = 120;

		Vector3 shakePos;

		// 煙カウント
		uint32_t smokeCount = 0;

		// 煙タイム
		uint32_t smokeTimer = 0;
		uint32_t MaxSmokeTimer = 30;

		uint32_t smokeFlag[5];

		// 爆発フラグ
		bool isExplosion = false;

		bool enmey = true;

		// クールタイム
		uint32_t coolTime = 0;
		uint32_t MaxCoolTime = 65;

	};
	Die die_;

#pragma endregion // 死亡演出

	FollowCamera* follow_;

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
		//float stoppingTime = 0;
		//float MaxStoppingTime = 20;

		// 動きが止まってから回復する（浮く）までの時間
		float cooldownTime = 60;

		// 反動時間
		float recoilTime = 0;
		float MaxRecoilTime = 10;

		float attackPower = 10;
	};

	// 通常近距離攻撃
	struct AssaultAttack {
		// t補間用
		float transitionFactor = 0;
		float transitionFactorSpeed = 0.01f;
		//腕が生えてから回転開始までの時間）
		float armGrowthToSpinDelay = 0;
		float MaxArmGrowthToSpinDelay = 30.0f;
		// 追ってくる速度
		float speed = 0.3f;

		// 動きが止まるまでの時間
		float stoppingTime = 0;
		float MaxStoppingTime = 20;

		// ぐるぐるし続ける時間
		float assaultTime = 0;
		float MaxAssaultTime = 120.0f;

		// 移動方向
		Vector3 moveDirection{};


		// 動きが止まってから回復するまでの時間
		float cooldownTime = 60;

		// 反動時間
		float recoilTime = 0;
		float MaxRecoilTime = 10;

	};

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

	//死亡行動初期化
	void BehaviorDieInitialize();

	//死亡行動更新
	void BehaviorDieUpdate();


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

	void InitializeSound();

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


	void SetF(FollowCamera* f) { follow_ = f; };

	void Move(float speed, bool flag);

	// 弾の初期化
	void BulletInitialize(Vector3 pos);

	void NormalBulletInitialize(Vector3 pos);

	void Normal2BulletInitialize(Vector3 pos);




	// 弾更新
	void BulletUpdate();

	// 弾の初期化
	void StageArmInitialize(int num);

	void StageArmInitialize(int num,int i);

	// 雷の初期化
	void ThunderInitialize(Vector3 pos);
	 // 衝突処理
	void OnCollision(const Collider* _other);

	void StageMovementRestrictions();

	bool IsStageMovementRestrictions();

	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();

public:
	void SetPlayer(Player* player) { player_ = player; };
	void SetStage(Stage* stage) { stage_ = stage; }
	const Behavior& GetBehavior() const { return behavior_; };
	const AttackBehavior& GetattackBehavior() const { return attackBehavior_; };
	const SpecialAttack& GetSpecialAttack() const { return specialAttackBehavior_; };
	const NormalAttack& GetNormalAttack() const { return normalAttackBehavior_; };

	float GetHPRatio()const { return (float)hp / (float)MaxHp; }

	void SetLight(LightGroup* _ptr);

	const float& GetDamege() const { return damage_; }

    bool isPostKillEffectFinished() const { return die_.coolTime >= die_.MaxCoolTime; }

private:
	// モデル
	Model* model_ = nullptr;
	Model* modelLeftArm_ = nullptr;
	Model* modelRightArm_ = nullptr;
	// モデル
	Model* modelBullet_ = nullptr;
	Model* modelStageArm_ = nullptr;
	Model* modelThunder_ = nullptr;

	// 予測線
	Model* modelPrediction_ = nullptr;

	// 予測線位置
	WorldTransform worldPrediction_;
	ObjectColor colorPrediction_;

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

	// 死亡演出

	std::unique_ptr<EnemyDeathParticle> deashParticle_;
	std::unique_ptr<EnemyDeathParticle> deashExplosionParticle_;

	// 煙
	std::array<std::unique_ptr<EnemyDeathParticle>,5> deashSmokeParticle_;


    std::unique_ptr<Collider> bodyCollider_;
    std::unique_ptr<Collider> leftArmCollider_;
    std::unique_ptr<Collider> rightArmCollider_;

private:
	float damage_ = 0;

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
	uint32_t MaxHp = 100;
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
	SpinAttack normalAttackShot1_;
	AssaultAttack normalAttackShot2_;


	SpinAttack attack4_;

	// ヒットカラーの設定
    bool isHitColor_ = false;
    Vector4 hitColor_ = { 1,0,0,1 };

    // ダメージクールタイム
    bool isCoolTime_ = false;
    float damageCoolTimer_ = 0;
    float damageCoolMaxTime_ = 2;

	std::unique_ptr<Audio> audio_;
	std::unique_ptr<Audio> audio2_;
	std::unique_ptr<Audio> audio3_;
	std::unique_ptr<Audio> audio4_;
	std::unique_ptr<Audio> audio5_;

	// 音
	

	struct Sounds {
		Audio::Sound bossMove;				//移動
		Audio::Sound bossArmPrediction;		//予測線
		Audio::Sound bossArmStickOut;		//腕はえ
		Audio::Sound bossJumpUp;			// ジャンプ
		Audio::Sound bossGetOff;			// 地面突き
		Audio::Sound bossThunder;			// 雷
		Audio::Sound bossReleaseElectricity;// 撃つ瞬間

		Audio::Sound bossGrowArm;			// 腕はえ
		Audio::Sound bossRotate;			// 回り
		Audio::Sound bossStopMove;			// 止まる

		Audio::Sound bossDieTremble;		// ブルブル震えている間
		Audio::Sound bossDieExplotion;		// 爆発する
		Audio::Sound smoke;					// 煙
		Audio::Sound bossDamage;			// ダメージ

	};	
	Sounds sounds_;

};










