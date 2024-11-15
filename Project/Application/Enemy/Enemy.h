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
	const Camera* camera_ = nullptr;

	Player* player_;

	std::list<std::unique_ptr<EnemyBullet>> bullets_;
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


	// 浮遊ギミック媒介変数
	float floatingParameter_ = 0.0f;
	// 浮遊サイクル<frame>
	int floatingPeriod = 120;



	int hp = 100;
	bool isAlive = true;


	//
	int behaviorTimer_;
	int timre;
};

