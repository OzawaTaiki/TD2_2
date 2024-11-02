#include "Player.h"
#include "VectorFunction.h"
#include <imgui.h>

void Player::Initialize()
{

	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	model_ = Model::CreateFromObj("bunny.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });
}

void Player::Update()
{
	if (ImGui::BeginTabBar("player"))
	{
		if (ImGui::BeginTabItem("player"))
		{
			ImGui::DragFloat3("translate", &worldTransform_.transform_.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotate_.x, 0.01f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}



	if (behaviorRequest_) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			//BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			//BehaviorJumpInitialize();
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Behavior::kRoot: // 通常行動更新
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack: // 攻撃行動更新
		//BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		//BehaviorJumpUpdate(); // ジャンプ行動更新
		break;
	}



	worldTransform_.UpdateData();
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);
}

void Player::BehaviorRootInitialize()
{

}

void Player::BehaviorRootUpdate()
{
	// 速さ
	velocity_ = { 0, 0, 0 };
	speed = 0.2f;
	if (Input::GetInstance()->IsKeyPressed(DIK_A)) {
		velocity_.x = -1.0f;
	}
	if (Input::GetInstance()->IsKeyPressed(DIK_D)) {
		velocity_.x = 1.0f;
	}
	if (Input::GetInstance()->IsKeyPressed(DIK_W)) {
		velocity_.z = 1.0f;
	}
	if (Input::GetInstance()->IsKeyPressed(DIK_S)) {
		velocity_.z = -1.0f;
	}
	// 移動量に速さを反映
	velocity_ = Multiply(Normalize(velocity_), speed);

	worldTransform_.transform_ += velocity_;
}
