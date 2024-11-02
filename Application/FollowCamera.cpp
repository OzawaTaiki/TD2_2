#include "FollowCamera.h"
#include "MatrixFunction.h"
#include "VectorFunction.h"
#include"imgui.h"

void FollowCamera::Initialize()
{
	camera_.Initialize();
	
}

void FollowCamera::Update()
{
	if (ImGui::BeginTabBar("followCamera"))
	{
		if (ImGui::BeginTabItem("followCamera"))
		{
			ImGui::DragFloat3("translate", &camera_.translate_.x, 0.01f);
			ImGui::DragFloat3("rotate", &camera_.rotate_.x, 0.01f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	const float kRotateSpeed = 0.000003f;
	if (Input::GetInstance()->IsKeyPressed(DIK_RIGHT)) {
		camera_.rotate_.y -= 1.0f * kRotateSpeed;
	}else if (Input::GetInstance()->IsKeyPressed(DIK_LEFT)) {
		camera_.rotate_.y += 1.0f * kRotateSpeed;
	}

	// 追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f, 2.0f, -10.0f };

	Matrix4x4 matrix = MakeRotateYMatrix(camera_.rotate_.y);

	// オフセットをカメラの回転に合わせて回転させる
	offset = Transform(offset, matrix);

	// 座標をコピーしてオフセット分ずらす
	camera_.translate_ = Add(target_->transform_, offset);

}
