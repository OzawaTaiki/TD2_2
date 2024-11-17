#pragma once
#pragma once
//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"


class EnemyThunder
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Vector3& position, const Vector3& Velocity, Model* model);

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

	Model* model2_ = nullptr;

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
	int32_t deathTimer_ = kLifeTime;

	// デスフラグ
	bool isDead_ = false;
};

