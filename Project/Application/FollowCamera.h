#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ConfigManager.h"

class Enemy;

class FollowCamera
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	//追従対象をポインタで持つ
	void SetTarget(const WorldTransform* target) { target_ = target; };

	const Camera& GetCamera() { return camera_; };




	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	void SetRotateY(const float& y) { camera_.rotate_.y = y; };
	void SetFlag(const bool& y) { flag_ = y; };
	void SetT(const float& y) { t_ = y; };


private:
	bool flag_ = false;

	bool ff_ = false;

	Enemy* enemy_;

	// ビュープロジェクション
	Camera camera_;

	//追従対象
	const WorldTransform* target_ = nullptr;

	Vector3 offset_ = { 0.0f, 20.0f, -60.0f };


	Vector3 attackRotate_{ 1.0399999618530273f,0,0};
	Vector3 attackTranslate_{ 0,370.0f,-231.49f};

	Vector3 normalRot{};

	Vector3 startPos{};
	Vector3 startRot{};

	Vector3 targetPos{};
	Vector3 targetRot{};

	Vector3 oldPos;
	Vector3 oldRot;

	float t_;

};

