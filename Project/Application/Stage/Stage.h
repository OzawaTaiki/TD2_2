#pragma once

//Engine
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "Input.h"
#include "ObjectColor.h"
#include "MyLib.h"

/// <summary>
/// ステージ
/// </summary>
class Stage
{
public:

	enum StageNum {
		kFlont = 0,
		kBack,
		kRight,
		kLeft,
		kFloor,
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


	Vector3 GetWallFloor() { return worldFloor_.GetWorldPosition();}
	Vector3 GetWallFlont() { return worldWallFlont.GetWorldPosition();}
	Vector3 GetWallBack() { return worldWallBack.GetWorldPosition();}
	Vector3 GetWallLeft() { return worldWallLeft.GetWorldPosition();}
	Vector3 GetWallRight() { return worldWallRight.GetWorldPosition();}

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
