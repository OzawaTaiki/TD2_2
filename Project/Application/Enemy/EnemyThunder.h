#pragma once
#pragma once
//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"
#include "Collider.h"

class EnemyThunder
{
public:
	// 攻撃2
	struct AttackThunder
	{
		// 移動位置
		//Vector3 attackPos = { 0,50,0 };
		float attackMinPos = 0;
		float attackMaxPos = 50;
		// 上下移動切り替え
		int clock = 1;
		//　雷をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
		// 着地時間
		float landingTime;
		// 最大
		float MaxLandingTime = 100.0f;


		////
		// 予測が出てから攻撃するまでの時間(最大)
		float MaxAttackPreparationTime = 120.0f;
		// 雷が出る時間(最大)
		float MaxThunderStrikeTime = 30.0f;
		// 細→太が始まるまで時間
		float MaxThicknessStartTime = 10.0f;
		// 細→太くなるまでの時間
		float MaxExpandTime = 15.0f;
		// 最小サイズ
		float minSize = 1.0f;
		// 最大サイズ
		float maxSize = 5.0f;
		//攻撃力
		float attackPower = 1;
		// 一列行
		int num = 4;
		// 間隔
		float positionInterval = 20;
		// 消える時間
		float deathTimer_;
		float MaxDeathTimer_ = 20.0f;

	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& Velocity, Model* model,EnemyThunder::AttackThunder *attack);

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
private:
	float MinMaxSize(float &maxTime, float &time, float &size);
private:

	// モデル
	Model* model_ = nullptr;

	Model* model2_ = nullptr;

    std::unique_ptr<Collider> collider_ = nullptr;


	// モデルカラー
	ObjectColor color_;
	ObjectColor color2_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	WorldTransform worldTransform2_;

	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 4;

	// デスタイマー
	int32_t deathTimer_ = 0;// kLifeTime;

	// デスフラグ
	bool isDead_ = false;

	AttackThunder* attack_;

	// 予測が出てから攻撃するまでの時間
	float attackPreparationTime_ = 0.0f;
	// 細→太くなるまでの時間
	float expandTime_ = 0;
	//
	float thicknessStartTime = 0;
	// 雷が出る時間
	float thunderStrikeTime = 0;
};

