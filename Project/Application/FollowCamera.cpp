#include "FollowCamera.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include"imgui.h"
#include "Player.h"
void FollowCamera::Initialize()
{
	camera_.Initialize();
	camera_.rotate_ = { 0.34f,0,0 };


	ConfigManager::GetInstance()->LoadData();

	ConfigManager::GetInstance()->SetVariable("followCamera", "offset", &offset_);
	ConfigManager::GetInstance()->SetVariable("followCamera", "rotate", &camera_.rotate_);

}

void FollowCamera::Update()
{
	if (ImGui::BeginTabBar("followCamera"))
	{
		if (ImGui::BeginTabItem("followCamera"))
		{
			//ImGui::DragFloat3("translate", &camera_.translate_.x, 0.01f);
			ImGui::DragFloat3("offset", &offset_.x, 0.01f);
			ImGui::DragFloat3("rotate", &camera_.rotate_.x, 0.01f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	if (target_) {
		const float kRotateSpeed = 0.000003f *10000;
		if (Input::GetInstance()->IsKeyPressed(DIK_RIGHT)) {
			camera_.rotate_.y += 1.0f * kRotateSpeed;
		}
		else if (Input::GetInstance()->IsKeyPressed(DIK_LEFT)) {
			camera_.rotate_.y -= 1.0f * kRotateSpeed;
		}

		//camera_.rotate_.y = target_->rotate_.y;

		// 追従対象からカメラまでのオフセット
		Vector3 offset = offset_;

		Matrix4x4 matrix = MakeRotateYMatrix(camera_.rotate_.y);

		// オフセットをカメラの回転に合わせて回転させる
		offset = Transform(offset, matrix);

		// 座標をコピーしてオフセット分ずらす
		camera_.translate_ = Add(target_->transform_, offset);
	}

	camera_.UpdateMatrix();
}
