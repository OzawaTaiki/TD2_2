#include "Player.h"
#include "VectorFunction.h"
#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <numbers>


// 8方向の角度テーブル（関数外で定義）
constexpr float destinationRotationYTable[] = {
	std::numbers::pi_v<float> / 2.0f,                      // 右
	std::numbers::pi_v<float> *3.0f / 2.0f,               // 左
	0.0f,                                                  // 後ろ
	std::numbers::pi_v<float>,                             // 前
	std::numbers::pi_v<float> *1.0f / 4.0f,               // 右前
	std::numbers::pi_v<float> *7.0f / 4.0f,               // 右後ろ
	std::numbers::pi_v<float> *3.0f / 4.0f,               // 左前
	std::numbers::pi_v<float> *5.0f / 4.0f                // 左後ろ
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


float DegreesToRadians(float degrees) { return float(degrees * ((float)M_PI / 180.0)); }

Vector3 DegreesToRadians(Vector3 degrees) {
	Vector3 resurt;

	resurt.x = float(degrees.x * ((float)M_PI / 180.0));
	resurt.y = float(degrees.y * ((float)M_PI / 180.0));
	resurt.z = float(degrees.z * ((float)M_PI / 180.0));

	return resurt;
}

float RadiansToDegrees(float radians) { return float(radians * (180.0 / (float)M_PI)); }

Vector3 RadiansToDegrees(Vector3 radians) {
	Vector3 resurt;

	resurt.x = float(radians.x * (180.0 / (float)M_PI));
	resurt.y = float(radians.y * (180.0 / (float)M_PI));
	resurt.z = float(radians.z * (180.0 / (float)M_PI));

	return resurt;
}


void Player::Initialize()
{

	worldTransform_.Initialize();
	worldTransform_.transform_ = Vector3{ 0,0,0 };

	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize();
	weapon_->SetPosition(Vector3{ 0.0f, 0.5f, 1.0f });
	weapon_->GetWorldTransform().parent_ = &worldTransform_; // 本体が親

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
			Vector3 mat = Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
			ImGui::DragFloat3("translateMat", &mat.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotate_.x, 0.01f);
			ImGui::DragInt("recastTime", &recastTime, 0.01f);
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
			BehaviorAttackInitialize();
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
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		//BehaviorJumpUpdate(); // ジャンプ行動更新
		break;
	}

	// HP
	if (hp <= 0) {
		isAlive = false;
	}


	// ワールドトランスフォーム更新
	weapon_->UpdateWorldTransform();
	worldTransform_.UpdateData();
}

void Player::Draw(const Camera& camera)
{
	
	model_->Draw(worldTransform_, &camera, &color_);


	switch (behavior_) {
	case Behavior::kRoot:
	case Behavior::kJump:
	default:

		break;
	case Behavior::kAttack:
		weapon_->UpdateWorldTransform();
		weapon_->Draw(camera);
		break;
	}
}

void Player::BehaviorRootInitialize()
{
	weapon_->SetRotation(Vector3{ 0,0,0 });
	recastTime = 0;
}

void Player::BehaviorRootUpdate()
{
#pragma region Move

	// 入力による移動と方向設定
	LRDirection newDirection = lrDirection_;
	velocity_ = { 0.0f, 0.0f, 0.0f };
	speed = 0.2f;
	// 前後左右の入力判定
	bool pressedW = Input::GetInstance()->IsKeyPressed(DIK_W);
	bool pressedA = Input::GetInstance()->IsKeyPressed(DIK_A);
	bool pressedS = Input::GetInstance()->IsKeyPressed(DIK_S);
	bool pressedD = Input::GetInstance()->IsKeyPressed(DIK_D);
	bool pressedSPACE = Input::GetInstance()->IsKeyPressed(DIK_SPACE);

	// 移動方向の設定
	if (pressedW) {
		velocity_.z = 1.0f;
		if (pressedA) {
			velocity_.x = -1.0f;
			newDirection = LRDirection::kLeftFront;
			direction_.x = velocity_.x;
		}
		else if (pressedD) {
			velocity_.x = 1.0f;
			newDirection = LRDirection::kRightFront;
			direction_.x = velocity_.x;
		}
		else {
			newDirection = LRDirection::kFront;
			direction_.x = velocity_.x;
		}
		direction_.y = velocity_.z;
	}
	else if (pressedS) {
		velocity_.z = -1.0f;
		if (pressedA) {
			velocity_.x = -1.0f;
			newDirection = LRDirection::kLeftBack;
			direction_.x = velocity_.x;
		}
		else if (pressedD) {
			velocity_.x = 1.0f;
			newDirection = LRDirection::kRightBack;
			direction_.x = velocity_.x;
		}
		else {
			newDirection = LRDirection::kBack;
			direction_.x = velocity_.x;
		}
		direction_.y = velocity_.z;
	}
	else if (pressedA) {
		velocity_.x = -1.0f;
		newDirection = LRDirection::kLeft;
		direction_.x = velocity_.x;
		direction_.y = velocity_.z;
	}
	else if (pressedD) {
		velocity_.x = 1.0f;
		newDirection = LRDirection::kRight;
		direction_.x = velocity_.x;
		direction_.y = velocity_.z;
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

#pragma endregion

	recastTime++;
	if (pressedSPACE) {
		if (recastTime >= 40) {

			behaviorRequest_ = Behavior::kAttack;
		}
	}

}

void Player::BehaviorAttackInitialize()
{
	workAttack.attackParameter_ = 0;
	attackParameter = 0;
	workAttack.comboIndex = 0;
}

void Player::BehaviorAttackUpdate()
{
	bool pressedSPACET = Input::GetInstance()->IsKeyTriggered(DIK_SPACE);
	// コンボ上限に達していない
	if (workAttack.comboIndex < ComboNum - 1) {
		if (pressedSPACET) {
			// 攻撃ボタンをトリガーしたら
				// コンボ有効
			workAttack.comboNext = true;
		}
	}




	// コンボ段階によってモーションを分岐
	switch (workAttack.comboIndex) {
		// 0:右から反時計回り
	case 0:		
		// パラメータ
		AttackParameter();
		

		weapon_->SetRotationX(weapon_->GetRotationX() + DegreesToRadians(15));

		SetAttackCombo(15);
		break;
	case 1:	
		// パラメータ
		AttackParameter();
	

		weapon_->SetRotationX(weapon_->GetRotationX() + DegreesToRadians(12));
		weapon_->SetRotationZ(DegreesToRadians(45));
		
		SetAttackCombo(15);
		break;
	case 2:
		// パラメータ
		AttackParameter();
		

		weapon_->SetRotationX(weapon_->GetRotationX() + DegreesToRadians(12));
		weapon_->SetRotationZ(DegreesToRadians(-45));
		
		SetAttackCombo(15);
		break;
	case 3:
		// パラメータ
		AttackParameter();

		if (workAttack.attackParameter_ <= 5) {
			
			Vector3 move(direction_.x, 0, direction_.y);

			

			move = Normalize(move);

			move = move * 2.1f;

			worldTransform_.transform_ += move;
		}

		weapon_->SetRotationX(DegreesToRadians(90));
		
		SetAttackCombo(25);
		break;
	default:
		behavior_ = Behavior::kRoot;
	}

}

void Player::AttackParameter()
{
	uint16_t attackPeriod = 90;

	float pi = 3.141592f;

	// 1フレームでのパラメータ加算値
	const float step = 2.0f * pi / attackPeriod;

	// パラメータを1ステップ分加算
	attackParameter += step;
}

void Player::SetAttackCombo(int parameter)
{
	//  既定の時間経過で通常行動に戻る
	if (++workAttack.attackParameter_ >= parameter) {
		// コンボ継続なら次のコンボに進む
		if (workAttack.comboNext) {

			// コンボフラグをリセット
			workAttack.comboNext = false;
			// 攻撃の色々な変数をリセットする
			attackParameter = 0;
			workAttack.attackParameter_ = 0;
			if (!(workAttack.comboIndex == 0)) {
			}
			//weapon_->ContactRecordClear();
			// 各パーツの角度などを次のコンボ用に初期化
			
			// 武器位置設定
			weapon_->SetRotation(Vector3{0,0,0});
			weapon_->SetRotationX(0);
			weapon_->SetRotationY(0);
			weapon_->SetRotationZ(0);


			// コンボ加算
			workAttack.comboIndex++;
		}
		else {
			// 何もしなかったら通常行動に
			behaviorRequest_ = Behavior::kRoot;
		}
	}
}


