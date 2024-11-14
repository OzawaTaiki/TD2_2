#pragma once
#pragma once
#define _USE_MATH_DEFINES
#include<cmath>


//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"

// application
#include "EnemyBullet.h"


// 
#include <list>
#include <optional>

class Player;

class Enemy
{
public:
	// 振るまい
	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kJump,   // ジャンプ中
	};

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

	// 弾の初期化
	void BulletInitialize(Vector3 pos);

	// 弾更新
	void BulletUpdate();

private:
	//通常行動初期化
	void BehaviorRootInitialize();

	//通常行動更新
	void BehaviorRootUpdate();


	//攻撃行動初期化
	void BehaviorAttackInitialize();


	//攻撃行動更新
	void BehaviorAttackUpdate();


	// 浮遊ギミック初期化
	void InitializeFloatingGimmick();

	// 浮遊ギミック更新
	void UpdateFloatingGimmick();



public:
	void SetPlayer(Player* player) { player_ = player; };

	const Behavior& GetBehavior() const { return behavior_; };

	const Camera& GetCamera() { return attackCamera_; };
private:
	// モデル
	Model* model_ = nullptr;
	// モデル
	Model* modelBullet_ = nullptr;



	// モデルカラー
	ObjectColor color_;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	//
	Vector3 oldPos_;
	//
	const Camera* camera_ = nullptr;

	Player* player_;

	std::list<std::unique_ptr<EnemyBullet>> bullets_;




	Camera attackCamera_;
private:
	// 速度
	Vector3 velocity_ = {};

	float speed;

	
	//振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;


	// 浮遊ギミック媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊サイクル<frame>
	int floatingPeriod = 120;


	// hp
	int hp = 100;
	// 生死フラグ
	bool isAlive = true;
	// 行動タイマー
	int behaviorTimer_;

	
	struct Attack3 {
		//移動位置
		Vector3 attackPos = { 0,10,0 };
		// 上下移動切り替え
		int clock1 = 1;
		//　弾をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
	};
	Attack3 attack3_;

	struct Attack2
	{
		// 移動位置
		Vector3 attackPos = { 0,10,0 };
		// 上下移動切り替え
		int clock = 1;
		//　雷をうつ
		bool isBulletShot = 0;
		// t補間用
		float transitionFactor = 0;
	};


};




