#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"

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

private:
	Model* model_ = nullptr;
	WorldTransform worldTransform;
	ObjectColor color_;

	
};
