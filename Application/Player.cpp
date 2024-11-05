#include "Player.h"
#include "VectorFunction.h"
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <numbers>

// 8方向の角度テーブル（関数外で定義）
constexpr float destinationRotationYTable[] = {
	std::numbers::pi_v<float> *3.0f / 2.0f,               // 左
	std::numbers::pi_v<float> / 2.0f,                      // 右
	std::numbers::pi_v<float>,                             // 前
	0.0f,                                                  // 後ろ
	std::numbers::pi_v<float> *5.0f / 4.0f,               // 右前
	std::numbers::pi_v<float> *3.0f / 4.0f,               // 左前
	std::numbers::pi_v<float> *7.0f / 4.0f,               // 右後ろ
	std::numbers::pi_v<float> *1.0f / 4.0f                // 左後ろ
};

// 角度差を -π ～ +π の範囲に正規化する関数
float NormalizeAngleDifference(float angle) {
	while (angle > std::numbers::pi_v<float>) angle -= 2.0f * std::numbers::pi_v<float>;
	while (angle < -std::numbers::pi_v<float>) angle += 2.0f * std::numbers::pi_v<float>;
	return angle;
}

float EaseOut(float t, float str, float end) {

	float t2 = 1.0f - powf(1.0f - t, 2.0f);
	return (1.0f - t2) * str + t2 * end;
}

void Player::Initialize()
{

	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	model_ = Model::CreateFromObj("Arrow/Arrow.obj");

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
	// 入力による移動と方向設定
	LRDirection newDirection = lrDirection_;
	velocity_ = { 0.0f, 0.0f, 0.0f };
	speed = 0.2f;
	// 前後左右の入力判定
	bool pressedW = Input::GetInstance()->IsKeyPressed(DIK_W);
	bool pressedA = Input::GetInstance()->IsKeyPressed(DIK_A);
	bool pressedS = Input::GetInstance()->IsKeyPressed(DIK_S);
	bool pressedD = Input::GetInstance()->IsKeyPressed(DIK_D);

	// 移動方向の設定
	if (pressedW) {
		velocity_.z = 1.0f;
		if (pressedA) {
			velocity_.x = -1.0f;
			newDirection = LRDirection::kLeftFront;
		}
		else if (pressedD) {
			velocity_.x = 1.0f;
			newDirection = LRDirection::kRightFront;
		}
		else {
			newDirection = LRDirection::kFront;
		}
	}
	else if (pressedS) {
		velocity_.z = -1.0f;
		if (pressedA) {
			velocity_.x = -1.0f;
			newDirection = LRDirection::kLeftBack;
		}
		else if (pressedD) {
			velocity_.x = 1.0f;
			newDirection = LRDirection::kRightBack;
		}
		else {
			newDirection = LRDirection::kBack;
		}
	}
	else if (pressedA) {
		velocity_.x = -1.0f;
		newDirection = LRDirection::kLeft;
	}
	else if (pressedD) {
		velocity_.x = 1.0f;
		newDirection = LRDirection::kRight;
	}

	// 方向が変わったときに回転開始
	if (newDirection != lrDirection_) {
		lrDirection_ = newDirection;
		turnFirstRotationY_ = worldTransform_.rotate_.y;
		tureTimer_ = kTimeTurn;
	}

	// 移動量に速さを反映
	velocity_ = Multiply(Normalize(velocity_), speed);

	worldTransform_.transform_ += velocity_;


	// 旋回制御
	if (tureTimer_ > 0.0f) {
		tureTimer_ -= 1.0f / 60.0f;

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 角度差を最短経路で補間
		float angleDifference = NormalizeAngleDifference(destinationRotationY - worldTransform_.rotate_.y);
		float targetRotationY = worldTransform_.rotate_.y + EaseOut(tureTimer_, 0.0f, angleDifference);

		// 自キャラの角度を設定する
		worldTransform_.rotate_.y = targetRotationY;
	};
}
