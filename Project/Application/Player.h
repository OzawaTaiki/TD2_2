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
#include "PlayerDustParticle.h"


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

class Enemy;

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
	void OnCollision(const Collider* _other);

	// カメラのビュープロジェクション
	void SetCamera(const Camera* camera) { camera_ = camera; };
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	WorldTransform& GetWorldTransform() { return worldTransform_; };
	WorldTransform& GetWorldTransformBody() { return worldTransformBody_; };

	const float& GetRotateY() { return worldTransform_.rotate_.y; };

	void SetStage(Stage* stage) { stage_ = stage; }
	const float& GetDamege() const { return damage_; }

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

	//死亡行動初期化
	void BehaviorDieInitialize();

	//死亡行動更新
	void BehaviorDieUpdate();

	// 攻撃パラメータ
	void AttackParameter();
	// 攻撃コンボセット
	void SetAttackCombo(int parameter);

	// 移動時の傾き
    void TiltMotion();

    // ヒットカラーの更新
    void UpdateHitColor();


private:
	Enemy* enemy_;

	// モデル
	Model* model_ = nullptr;

	// モデルカラー
	ObjectColor color_;
    Vector4 defaultColor_ = { 1,1,1,1 };

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	WorldTransform worldTransformBody_;		//体

	//
	WorldTransform oldWorldTransform_;

	// すてーじ
	Stage* stage_;
	// 武器
	std::unique_ptr<Weapon> weapon_;

    std::unique_ptr< PlayerDustParticle> dustParticle_;

	std::array < std::unique_ptr< PlayerDustParticle>,2> smokeParticle_;

	//
	const Camera* camera_ = nullptr;

	// コライダー
    std::unique_ptr<Collider> collider_;

private:
	float damage_ = 0;
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
		kDie,       // 死亡状態
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
	uint32_t MaxRecastTime = 30;

	uint32_t maxHp = 100;
	int hp = 100;
	bool isAlive = true;

	// 移動フラグ
    bool isMove_ = false;
	// 最大傾き:設定
    float tiltMotionMaxRotate_ = 0.0f;
    // 最大傾きまでににかかる時間(秒):設定
    float tiltMotionDuration_ = 1.0f;
	// 傾きタイマー
    float tiltMotionTimer_ = 0.0f;
    // 傾きの速度:計算
    float tiltMotionSpeed_ = 0.0f;

    // カラー変更フラグ
    bool isHitColor_ = false;
    // タイマー
    float hitColorTimer_ = 0.0f;
    // カラーを変更する時間
    float hitColorDuration_ = 0.1f;
    // ヒットカラー
    Vector4 hitColor_ = { 1,0,0,1 };


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

		bool player = true;

		// クールタイム
		uint32_t coolTime = 0;
		uint32_t MaxCoolTime = 65;

		// t補間用
		float transitionFactor = 0;
		float transitionFactorSpeed = 0.01f;

		Vector3 strRotate;

	};
	Die die_;

#pragma endregion // 死亡演出
};
