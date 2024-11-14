#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"

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
	
	
private:

	// ビュープロジェクション
	Camera camera_;

	//追従対象
	const WorldTransform* target_ = nullptr;

};

