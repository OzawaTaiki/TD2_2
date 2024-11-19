#include "Player.h"
#include "VectorFunction.h"
#include "../Collider/CollisionManager.h"
#include <imgui.h>

#include "../../../Application/Stage/Stage.h"

#include <algorithm>
#include <cassert>
#include <numbers>
#include <MatrixFunction.h>


// 8方向の角度テーブル（関数外で定義）
constexpr float destinationRotationYTable[] = {
	std::numbers::pi_v<float> / 2.0f,                      // 右
	std::numbers::pi_v<float> * 3.0f / 2.0f,               // 左
	0.0f,                                                  // 後ろ
	std::numbers::pi_v<float>,	 // 前
	std::numbers::pi_v<float> * 1.0f / 4.0f,               // 右前
	std::numbers::pi_v<float> * 7.0f / 4.0f,               // 右後ろ
	std::numbers::pi_v<float> * 3.0f / 4.0f,               // 左前
	std::numbers::pi_v<float> * 5.0f / 4.0f,                // 左後ろ
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

	oldWorldTransform_.Initialize();
	oldWorldTransform_ = worldTransform_;

	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize();
	weapon_->SetPosition(Vector3{ 0.0f, 0.5f, 1.0f });
	weapon_->GetWorldTransform().parent_ = &worldTransform_; // 本体が親

	model_ = Model::CreateFromObj("playerBody/playerBody.obj");


	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

    dustParticle_ = std::make_unique<PlayerDustParticle>();
    dustParticle_->Initialize();
	dustParticle_->SetPlayerMat(&worldTransform_);

	ConfigManager::GetInstance()->SetVariable("Player","speed",&speed);
	ConfigManager::GetInstance()->SetVariable("Player", "tiltMotionRotate", &tiltMotionMaxRotate_);
    ConfigManager::GetInstance()->SetVariable("Player", "tileMotionDuration", &tiltMotionDuration_);
}

void Player::Update()
{

	if (ImGui::BeginTabBar("GameScene"))
	{
		if (ImGui::BeginTabItem("player"))
		{
			ImGui::DragFloat3("translate", &worldTransform_.transform_.x, 0.01f);
			Vector3 mat = Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
			ImGui::DragFloat3("translateMat", &mat.x, 0.01f);
			ImGui::DragFloat3("rotate", &worldTransform_.rotate_.x, 0.01f);
			ImGui::DragInt("recastTime", &recastTime, 0.01f);
			ImGui::DragFloat("speed", &speed, 0.01f);
			ImGui::SliderAngle("tiltMotion", &tiltMotionMaxRotate_, 0, 45.0f);
			ImGui::DragFloat("tiltMotionDuration", &tiltMotionDuration_, 0.01f, 0.0f, 10.0f);
			if (ImGui::Button("save"))
			{
				ConfigManager::GetInstance()->SaveData("Player");
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();

	}


	StageMovementRestrictions();


	//oldWorldTransform_ = worldTransform_;


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

	dustParticle_->Update(isMove_);

	// ワールドトランスフォーム更新
	weapon_->UpdateWorldTransform();
	worldTransform_.UpdateData();
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

void Player::OnCollision()
{
}

void Player::StageMovementRestrictions()
{
	if (stage_->GetWallBack().z < worldTransform_.GetWorldPosition().z) {
		worldTransform_.transform_.z = stage_->GetWallBack().z ;
	}
	else if (stage_->GetWallFlont().z > worldTransform_.GetWorldPosition().z) {
		worldTransform_.transform_.z = stage_->GetWallFlont().z ;
	}
	if (stage_->GetWallLeft().x > worldTransform_.GetWorldPosition().x) {
		worldTransform_.transform_.x = stage_->GetWallLeft().x;
	}
	else if (stage_->GetWallRight().x < worldTransform_.GetWorldPosition().x) {
		worldTransform_.transform_.x = stage_->GetWallRight().x ;
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

	isMove_ = false;
	LRDirection newDirection = lrDirection_;
	velocity_ = { 0.0f, 0.0f, 0.0f };
	//speed = 0.2f;

	// 入力判定
	bool pressedW = Input::GetInstance()->IsKeyPressed(DIK_W);
	bool pressedA = Input::GetInstance()->IsKeyPressed(DIK_A);
	bool pressedS = Input::GetInstance()->IsKeyPressed(DIK_S);
	bool pressedD = Input::GetInstance()->IsKeyPressed(DIK_D);
	bool pressedSPACE = Input::GetInstance()->IsKeyPressed(DIK_SPACE);

	// 入力方向ベクトルの初期化
	Vector3 inputDirection = { 0.0f, 0.0f, 0.0f };
	if (pressedW) {
		inputDirection.z += 1.0f;
	}
	if (pressedS) {
		inputDirection.z -= 1.0f;
	}
	if (pressedA) {
		inputDirection.x -= 1.0f;
	}
	if (pressedD) {
		inputDirection.x += 1.0f;
	}

    if (pressedW || pressedA || pressedS || pressedD) isMove_ = true;

	// 入力がある場合、カメラの向きに基づいた移動方向を計算
	if (inputDirection.x != 0.0f || inputDirection.z != 0.0f) {
		// 入力方向を正規化
		inputDirection = Normalize(inputDirection);

		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
		Matrix4x4 cameraWorldMatrix = Inverse(camera_->matView_);

		// カメラの向きに基づいて移動方向をワールド座標系に変換
		Vector3 worldDirection = {
			inputDirection.x * cameraWorldMatrix.m[0][0] + inputDirection.z * cameraWorldMatrix.m[2][0],
			0.0f,
			inputDirection.x * cameraWorldMatrix.m[0][2] + inputDirection.z * cameraWorldMatrix.m[2][2]
		};

		velocity_ = Normalize(worldDirection) * speed;

		// 方向設定
		direction_.x = velocity_.x;
		direction_.y = velocity_.z;

		// プレイヤーの向きを設定
		if (inputDirection.z > 0) {
			newDirection = (inputDirection.x < 0) ? LRDirection::kLeftFront : (inputDirection.x > 0) ? LRDirection::kRightFront : LRDirection::kFront;
		}
		else if (inputDirection.z < 0) {
			newDirection = (inputDirection.x < 0) ? LRDirection::kLeftBack : (inputDirection.x > 0) ? LRDirection::kRightBack : LRDirection::kBack;
		}
		else {
			newDirection = (inputDirection.x < 0) ? LRDirection::kLeft : LRDirection::kRight;
		}
	}

	// 方向が変わったときに回転開始
	if (newDirection != lrDirection_) {
		lrDirection_ = newDirection;
		turnFirstRotationY_ = worldTransform_.rotate_.y;
		tureTimer_ = kTimeTurn;
	}

	// 移動量に速さを反映
	worldTransform_.transform_ += velocity_;

	// 移動時の傾き
	TiltMotion();

	// 旋回制御
	if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
		// 移動中なら常に旋回処理を行う
		tureTimer_ = kTimeTurn;

		// 状態に応じた目的の角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// カメラの向きを考慮して目的の回転角度を補正
		Matrix4x4 cameraWorldMatrix = Inverse(camera_->matView_);

		// カメラのY軸回転角度（ワールド座標系）を取得
		float cameraYaw = atan2f(cameraWorldMatrix.m[2][0], cameraWorldMatrix.m[2][2]);

		// 目的の角度にカメラの向きを加算して補正
		float adjustedRotationY = destinationRotationY + cameraYaw;

		// 角度差を最短経路で補間
		float angleDifference = NormalizeAngleDifference(adjustedRotationY - worldTransform_.rotate_.y);
		float targetRotationY = worldTransform_.rotate_.y + EaseOut(tureTimer_, 0.0f, angleDifference);

		// プレイヤーの回転を更新
		worldTransform_.rotate_.y = targetRotationY;
	}
	else if (tureTimer_ > 0.0f) {
		// 停止中でも旋回が残っていれば処理を継続
		tureTimer_ -= 1.0f / 60.0f;

		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		Matrix4x4 cameraWorldMatrix = Inverse(camera_->matView_);
		float cameraYaw = atan2f(cameraWorldMatrix.m[2][0], cameraWorldMatrix.m[2][2]);
		float adjustedRotationY = destinationRotationY + cameraYaw;

		float angleDifference = NormalizeAngleDifference(adjustedRotationY - worldTransform_.rotate_.y);
		float targetRotationY = worldTransform_.rotate_.y + EaseOut(tureTimer_, 0.0f, angleDifference);

		worldTransform_.rotate_.y = targetRotationY;
	}


#pragma endregion

	recastTime++;
	if (pressedSPACE) {
		if (recastTime >= 30) {
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

			move = move * 1.1f;

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

void Player::TiltMotion()
{
	if (!isMove_)
	{
		tiltMotionTimer_ -= 1.0f / 15.0f;
        if (tiltMotionTimer_ <= 0.0f)
        {
            tiltMotionTimer_ = 0.0f;
        }
		worldTransform_.rotate_.x = Lerp(0.0f, tiltMotionMaxRotate_, tiltMotionTimer_ / tiltMotionDuration_);

		return;
	}

    tiltMotionTimer_ += 1.0f / 60.0f;
    if (tiltMotionTimer_ >= tiltMotionDuration_)
    {
        tiltMotionTimer_ = tiltMotionDuration_;
    }

	float t = tiltMotionTimer_ / tiltMotionDuration_;
	t = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
	worldTransform_.rotate_.x = Lerp(0.0f, tiltMotionMaxRotate_, t);

}


