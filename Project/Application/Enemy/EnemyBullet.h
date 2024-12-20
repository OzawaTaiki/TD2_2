#pragma once
//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"
#include "Collider.h"


class EnemyBullet
{
public:
	// 攻撃3
	struct Bullet {
		//移動位置
		
		// 最高度
		float MaxPosY = 10;
		// 最低位置
		float MinPosY = 0;

		// 最高高度にいる時間
		float MaxYTime = 20;
		float maxYTime = 0;

		// 最低位置にいる時間
		float MinYTime = 20;
		float minYTime = 0;



		// 上下移動切り替え
		int clock1 = 1;
		//　弾をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
		// プレイヤーのold位置
		Vector3 oldTraget{};
		// start



		// 弾のスピード
		float speed = 0.3f;
		// 球の個数
		uint32_t numElectricCount = 16;
		// 1フェーズでの回数
		int numShotsPerPhase = 0;
		int MaxNumShotsPerPhase = 4;
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

    void Oncollision(const Collider* _other);
	
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

	// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;


};

Vector3 StartEnd(Vector3& start, Vector3& end, float& t);
float StartEnd(float& start, float& end, float& t);


