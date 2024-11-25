#include "FollowCamera.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include "ImGuiManager.h"
#include "Player.h"
#include "Enemy/Enemy.h"

#include <numbers>

template<typename T>
T TLerp(const T& a, const T& b, float t) {
	return a * (1.0f - t) + b * t;
}

float TShortestAngleLerp(float start, float end, float factor) {
	float difference = fmod(end - start, 360.0f); if (difference > 180.0f) {
		difference -= 360.0f;
	}
	else if (difference < -180.0f) {
		difference += 360.0f;
	} return start + factor * difference;
}

// 角度差を -π ～ +π の範囲に正規化する関数
float TNormalizeAngleDifference(float angle) {
	while (angle > std::numbers::pi_v<float>) angle -= 2.0f * std::numbers::pi_v<float>;
	while (angle < -std::numbers::pi_v<float>) angle += 2.0f * std::numbers::pi_v<float>;
	return angle;
}


template<typename T>
T TEaseOut(const T& a, const T& b, float t) {
	float t2 = 1.0f - powf(1.0f - t, 2.0f);
	return (1.0f - t2) * a + t2 * b;
}


// start: 開始値
// end: 終了値
// factor: 0から1の範囲の補間係数
template<typename T>
T easeInExpo(const T& a, const T& b, float t){
	if (t == 0) {
		return a;
	}
	Vector3 c = b - a;
	return c * std::powf(2, 10 * (t - 1)) + a;
}

// easeOutExpo関数
template<typename T>
T easeOutExpo(const T& start, const T& end, float factor) {
	if (factor == 1.0f) {
		return end;
	}
	Vector3 result;
	result.x = (end.x - start.x) * (-std::powf(2, -10 * factor) + 1) + start.x;
	result.y = (end.y - start.y) * (-std::powf(2, -10 * factor) + 1) + start.y;
	result.z = (end.z - start.z) * (-std::powf(2, -10 * factor) + 1) + start.z;
	return result;
}
void FollowCamera::Initialize()
{
	camera_.Initialize();
	camera_.rotate_ = { 0.34f,0,0 };

	targetPos = oldPos;
	targetRot = oldRot;


	ConfigManager::GetInstance()->SetVariable("followCamera", "offset", &offset_);
	ConfigManager::GetInstance()->SetVariable("followCamera", "rotate", &camera_.rotate_);

	startPos = camera_.translate_;
	startRot = camera_.rotate_;


	targetPos = camera_.translate_;
	targetRot = camera_.rotate_;

	t_ = 1.0f;
}

void FollowCamera::Update()
{

	camera_.Update(0);
#ifdef _DEBUG


	if (ImGui::BeginTabBar("GameScene"))
	{
		if (ImGui::BeginTabItem("followCamera"))
		{
			ImGui::DragFloat3("offset", &offset_.x, 0.01f);
			ImGui::DragFloat3("rotate", &camera_.rotate_.x, 0.01f);

			camera_.ShakeParametaerSettingFromImGui();

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
#endif // _DEBUG

	static float t = 0.01f;

	if (flag_) {
		t_ += t;

		// 追従対象からカメラまでのオフセット
		Vector3 offset = offset_;

		Matrix4x4 matrix = MakeRotateYMatrix(camera_.rotate_.y);

		// オフセットをカメラの回転に合わせて回転させる
		offset = Transform(offset, matrix);

		// 座標をコピーしてオフセット分ずらす

		oldPos = Add(target_->transform_, offset);

		if (t_ >= 1.0f) {
			t_ = 1.0f;

			targetPos = oldPos;
			targetRot = oldRot;
			oldRot.y = 0;

		}
		else {
			camera_.translate_ = easeInExpo(startPos, attackTranslate_, t_);

			camera_.rotate_.x = TShortestAngleLerp(startRot.x, attackRotate_.x, t_);
			camera_.rotate_.z = TShortestAngleLerp(startRot.z, attackRotate_.z, t_);
			camera_.rotate_.y = TEaseOut(DegreesToRadians(startRot.y), DegreesToRadians(attackRotate_.y), t_);

		}

        //camera_.translate_ = Add(target_->transform_, offset) + camera_.GetShakeOffset();

	}
	else {
		t_ += t;

		if (t_ > 1.0f) {

			const float kRotateSpeed = 0.000003f * 10000;

			if (Input::GetInstance()->IsControllerConnected()) {
				camera_.rotate_.y += Input::GetInstance()->GetPadRightStick().x * kRotateSpeed;
			}
			else {

				if (Input::GetInstance()->IsKeyPressed(DIK_RIGHT)) {
					camera_.rotate_.y += 1.0f * kRotateSpeed;
				}
				else if (Input::GetInstance()->IsKeyPressed(DIK_LEFT)) {
					camera_.rotate_.y -= 1.0f * kRotateSpeed;
				}
			}

			//camera_.rotate_.y = fmod(camera_.rotate_.y, DegreesToRadians(360.0f));
			if (camera_.rotate_.y < -1) {
				camera_.rotate_.y += DegreesToRadians(360.0f);
			}
			if (camera_.rotate_.y > 1) {
				camera_.rotate_.y -= DegreesToRadians(360.0f);
			}


			if (target_) {
				// 追従対象からカメラまでのオフセット
				Vector3 offset = offset_;

				Matrix4x4 matrix = MakeRotateYMatrix(camera_.rotate_.y);

				// オフセットをカメラの回転に合わせて回転させる
				offset = Transform(offset, matrix);


				// 座標をコピーしてオフセット分ずらす
				camera_.translate_ = Add(target_->transform_, offset);

				startPos = { Add(target_->transform_,Vector3(0,20,-60)) };



				oldRot = camera_.rotate_;
				oldPos = camera_.translate_;


				startRot = camera_.rotate_;
			}



		}
		else {


			camera_.translate_ = easeOutExpo(attackTranslate_, targetPos, t_);

			camera_.rotate_.x = TShortestAngleLerp((attackRotate_.x), (targetRot.x), t_);
			camera_.rotate_.y = TShortestAngleLerp((attackRotate_.y), (targetRot.y), t_);
			camera_.rotate_.z = TShortestAngleLerp((attackRotate_.z), (targetRot.z), t_);

		}

	}
	camera_.UpdateMatrix();
}
