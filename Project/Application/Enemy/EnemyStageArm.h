#pragma once
#pragma once
//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"

#include "Collider.h"


class EnemyStageArm
{
public:
	// 攻撃1
	struct AttackArm {
		//敵の移動位置
		Vector3 attackPos = { 0,10,0 };
		// 上下移動切り替え
		int clock1 = 1;
		//　弾をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
		// 着地時間
		float landingTime;
		// 最大着地時間
		float MaxLandingTime = 300.0f;

		/// アーム関係
		// 攻撃出現場所
		//int attackSpawnLocation = 1;
		// 前回腕の出た場所
		//int oldAttackSpawnLocation = attackSpawnLocation;
		// 出る腕の本数
		int armNum = 0;
		// 出る腕の本数(最大値)
		int MaxArmNum = 5;
		// 予測が出てから攻撃するまでの時間(最大)
		float MaxAttackPreparationTime = 120.0f;
		// 点滅
		float Maxblinking = 30.0f;

		// 伸びる時間(最大)
		//float MaxAttaskMoveTime = 60.0f;
		// 伸びる長さ最大
		float MaxLength = 30;

		// 腕の突き出す速度
		float armSpeed = 0.5f;
		// 攻撃してから次の予測線が出る時間
		float AttackToNextPredictionDelay = 0.0f;
		// 攻撃してから次の予測線が出る時間(最大)
		float MaxAttackToNextPredictionDelay = 30.0f;
		// 引っ込むまでの時間(最大)
		float MaxArmRetractTime = 30.0f;
		//　弱点がついた腕が出てくる確率
		float weakArmSpawnProbability = 10.0f;
		// 攻撃力
		float attackPower = 1;

		// 位置
		std::vector<int>availableLocations;

		// 攻撃方法変更
		int randAttack;
		// 
		int rrr;

	};


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& Velocity, Model* model, EnemyStageArm::AttackArm* attack);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const Camera& camera);

	bool IsDead() const { return isDead_; };

	void SetParent(const WorldTransform* parent);

    void OnCollision(const Collider* collider);
private:
	
	AttackArm* attack_;


	// モデル
	Model* model_ = nullptr;
	
	Model* model2_ = nullptr;

    std::unique_ptr<Collider> collider_ ;
	
	// モデルカラー
	ObjectColor color_;
	ObjectColor color2_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	WorldTransform worldTransform2_;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 6;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;

	// デスフラグ
	bool isDead_ = false;

	// 予測が出てから攻撃するまでの時間
	float attackPreparationTime = 0.0f;
	// 伸びる時間
	//float attaskMoveTime = 0.0f;
	// 引っ込むまでの時間
	float armRetractTime = 0.0f;
	// 伸びる長さ
	float length = 0;
};