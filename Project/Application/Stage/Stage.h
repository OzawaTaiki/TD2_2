#pragma once

//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"


/// <summary>
/// ステージ
/// </summary>
class Stage
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


private:
	// モデル
	Model* modelFloor_ = nullptr;
	Model* modelWallFlont_ = nullptr;
	Model* modelWallRight_ = nullptr;
	Model* modelWallLeft_ = nullptr;
	Model* modelWallBack_ = nullptr;

	// モデルカラー
	ObjectColor color_;

	// 床
	WorldTransform worldFloor_;


	// 壁
	WorldTransform worldWallFlont;
	WorldTransform worldWallRight;
	WorldTransform worldWallLeft;
	WorldTransform worldWallBack;


};
