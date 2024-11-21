#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "ObjectColor.h"
#include "../Collider/Collider.h"
#include "ParticleEmitters.h"

#include <memory>

class Weapon
{
public:
	//　更新
	void Initialize();

	// ワールドトランスフォームの初期化
	void UpdateWorldTransform();
	// 描画
	void Draw(const Camera& viewProjection);


	// 接触履歴を抹消
	void ContactRecordClear();

	void RegisterCollider();
	void OnCollision(const Collider* _other);



	// ゲッター、セッター
public:
	// ポジション
	void SetPosition(const Vector3& translation) { worldTransform_.transform_ = translation; };

	//
	const Vector3& GetRotation() const { return worldTransform_.rotate_; }
	const float& GetRotationX() const { return worldTransform_.rotate_.x; }
	const float& GetRotationY() const { return worldTransform_.rotate_.y; }
	const float& GetRotationZ() const { return worldTransform_.rotate_.z; }
	void SetRotation(const Vector3& rotation) { worldTransform_.rotate_ = rotation; }
	void SetRotationX(const float& rotation) { worldTransform_.rotate_.x = rotation; }
	void SetRotationY(const float& rotation) { worldTransform_.rotate_.y = rotation; }
	void SetRotationZ(const float& rotation) { worldTransform_.rotate_.z = rotation; }

	// ワールドトランスフォーム
	WorldTransform& GetWorldTransform() { return worldTransform_; };


private:
	// ハンマー武器
	Model* model_;

	WorldTransform worldTransform_;

	// モデルカラー
	ObjectColor color_;

    std::unique_ptr<Collider> collider_;

    std::unique_ptr<ParticleEmitter> hitPatricles_ = nullptr;
    std::unique_ptr<ParticleEmitter> hitPatricles2_ = nullptr;

};

