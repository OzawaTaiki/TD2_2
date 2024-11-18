#pragma once
//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"


class EnemyBullet
{
public:
	// 攻撃3
	struct Bullet {
		//移動位置
		Vector3 attackPos = { 0,10,0 };

		// 最高度
		float MaxPosY;

		// 上下移動切り替え
		int clock1 = 1;
		//　弾をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
		// start



		// 弾のスピード
		float speed = 0.3f;
		// 球の個数
		int numElectricCount = 16;
		// 1フェーズでの回数
		int numShotsPerPhase = 0;
		int MaxNumShotsPerPhase = 3;
		//次の攻撃までの時間
		float attackCooldown = 0.0f;
		float MaxAttackCooldown = 20.0f;
		// 攻撃力
		float attackPower = 1;
	};
	

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& Velocity, Model* model,EnemyBullet::Bullet *bullet);

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
	// モデル
	Model* model_ = nullptr;

	Bullet* bullet_;

	// モデルカラー
	ObjectColor color_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;


	// 速度
	Vector3 velocity_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;

	// デスフラグ
	bool isDead_ = false;



};

Vector3 StartEnd(Vector3& start, Vector3& end, float& t);
float StartEnd(float& start, float& end, float& t);


