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
#include "MyLib.h"
#include "../Collider/Collider.h"

//Application
#include "Weapon.h"


// 
#include <list>
#include <optional>


class Stage;

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

    /// <summary>
    /// 衝突処理
    /// </summary>
	void OnCollision();

	// カメラのビュープロジェクション
	void SetCamera(const Camera* camera) { camera_ = camera; };


	WorldTransform& GetWorldTransform() { return worldTransform_; };

	const float& GetRotateY() { return worldTransform_.rotate_.y; };

	void SetStage(Stage* stage) { stage_ = stage; }


	// 移動制限
	void StageMovementRestrictions();

private:


	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();


	//攻撃行動初期化
	void BehaviorAttackInitialize();

	
	//攻撃行動更新
	void BehaviorAttackUpdate();

	// 攻撃パラメータ
	void AttackParameter();
	// 攻撃コンボセット
	void SetAttackCombo(int parameter);


	

private:
	// モデル
	Model* model_ = nullptr;
	
	// モデルカラー
	ObjectColor color_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	//
	WorldTransform oldWorldTransform_;

	// すてーじ
	Stage* stage_;
	// 武器
	std::unique_ptr<Weapon> weapon_;

    // コライダー    
	std::unique_ptr<Collider> collider_ = nullptr;


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


	// 攻撃用ワーク
	struct WrokAttack {
		// 攻撃ギミックの媒介変数
		int attackParameter_ = 0;
		int comboIndex = 0;
		int inComboPhase = 0;
		bool comboNext = false;
		int comboSwitchingNum = 0;
		Vector2 joyDirection;
	};
	WrokAttack workAttack{};

	struct Direction
	{

	}; 
	Vector2 direction_;

	float attackParameter;
	// コンボの数
	static const int ComboNum = 4;

	// 攻撃再発動時間
	int recastTime = 0;


	int hp = 100;
	bool isAlive = true;
};
