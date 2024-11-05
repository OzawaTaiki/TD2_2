#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"

#include <list>
#include <optional>


// 8方向の列挙型
enum class LRDirection {
	kRight,         // 右
	kLeft,          // 左
	kFront,         // 前
	kBack,          // 後ろ
	kRightFront,    // 右前
	kLeftFront,     // 左前
	kRightBack,     // 右後ろ
	kLeftBack       // 左後ろ
};

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public :

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


	// カメラのビュープロジェクション
	void SetCamera(const Camera* camera) { camera_ = camera; };


	WorldTransform& GetWorldTransform() { return worldTransform_; };

private:


	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();


	//攻撃行動初期化
	//void BehaviorAttackInitialize();

	
	//攻撃行動更新
	//void BehaviorAttackUpdate();



private:
	// モデル
	Model* model_ = nullptr;
	
	// モデルカラー
	ObjectColor color_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	//
	const Camera* camera_ = nullptr;
private:
	//左右
	LRDirection lrDirection_ = LRDirection::kRight;
	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float tureTimer_ = 0.0f;
	//旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 速度
	Vector3 velocity_ = {};

	float speed;
	// 振るまい
	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kJump,   // ジャンプ中
	};
	//振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
};
