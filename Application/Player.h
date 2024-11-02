#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"

#include <list>
#include <optional>
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
