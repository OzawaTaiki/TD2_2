#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ConfigManager.h"

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
	
	void SetRotateY(const float& y) { camera_.rotate_.y = y; };

private:

	// ビュープロジェクション
	Camera camera_;

	//追従対象
	const WorldTransform* target_ = nullptr;

	Vector3 offset_ = { 0.0f, 20.0f, -60.0f };

};

