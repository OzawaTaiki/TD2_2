#include "Stage.h"
#include "../Enemy/Enemy.h"

void Stage::Initialize()
{
	// 床
	worldFloor_.Initialize();
	worldFloor_.transform_ = Vector3{ 0,-3.0f,0 };
	worldFloor_.rotate_.y = { 1.57f };

	// 床モデル
	modelFloor_ = Model::CreateFromObj("Tile/Tile.gltf");

	// 壁(前)
	worldWallFlont.Initialize();
	worldWallFlont.transform_ = Vector3{ 0,0,-100 };
	worldWallFlont.rotate_ = Vector3{ 0,0,0 };

	worldWallBack.Initialize();
	worldWallBack.transform_ = Vector3{ 0,0,100 };
	worldWallBack.rotate_ = Vector3{ 0,-3.14,0 };

	worldWallRight.Initialize();
	worldWallRight.transform_ = Vector3{ 100,0,0 };
	worldWallRight.rotate_ = Vector3{ 0,DegreesToRadians(-90),0 };

	worldWallLeft.Initialize();
	worldWallLeft.transform_ = Vector3{ -100,0,0 };
	worldWallLeft.rotate_ = Vector3{ 0,DegreesToRadians(90),0 };

	modelWallFlont_ = Model::CreateFromObj("wall/wall.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

	ConfigManager::GetInstance()->SetVariable("stage", "worldFloor_translate", &worldFloor_.transform_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldFloor_rotate", &worldFloor_.rotate_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallFlont_translate", &worldWallFlont.transform_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallFlont_rotate", &worldWallFlont.rotate_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallBack_translate", &worldWallBack.transform_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallBack_rotate", &worldWallBack.rotate_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallLeft_translate", &worldWallLeft.transform_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallLeft_rotate", &worldWallLeft.rotate_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallRight_translate", &worldWallRight.transform_);
	ConfigManager::GetInstance()->SetVariable("stage", "worldWallRight_rotate", &worldWallRight.rotate_);


}

void Stage::Update()
{
#ifdef _DEBUG

	if (ImGui::BeginTabBar("GameScene"))
	{
		if (ImGui::BeginTabItem("stage"))
		{


			ImGui::DragFloat3("flontTranslate", &worldWallFlont.transform_.x, 0.1f);
			ImGui::DragFloat3("flontRotate", &worldWallFlont.rotate_.x, 0.1f);
			ImGui::DragFloat3("backTranslate", &worldWallBack.transform_.x, 0.1f);
			ImGui::DragFloat3("backRotate", &worldWallBack.rotate_.x, 0.1f);
			ImGui::DragFloat3("rightTranslate", &worldWallRight.transform_.x, 0.1f);
			ImGui::DragFloat3("rightTrotate", &worldWallRight.rotate_.x, 0.1f);
			ImGui::DragFloat3("leftTranslate", &worldWallLeft.transform_.x, 0.1f);
			ImGui::DragFloat3("leftTotate", &worldWallLeft.rotate_.x, 0.1f);
			ImGui::DragFloat3("floorTranslate", &worldFloor_.transform_.x, 0.1f);
			ImGui::DragFloat3("floorRotate", &worldFloor_.rotate_.x, 0.1f);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
#endif // _DEBUG


	worldWallFlont.UpdateData();

	worldFloor_.UpdateData();
	worldWallBack.UpdateData();
	worldWallLeft.UpdateData();
	worldWallRight.UpdateData();
}

void Stage::Draw(const Camera& camera)
{
	// モデル
	modelFloor_->Draw(worldFloor_, &camera, &color_);

	// 壁(前)
	if ((enemy_->GetBehavior() != Enemy::Behavior::kAttack) && (enemy_->GetattackBehavior() != Enemy::AttackBehavior::kSpecial) && (enemy_->GetSpecialAttack() != Enemy::SpecialAttack::kAttack2)) {

		modelWallFlont_->Draw(worldWallFlont, &camera, &color_);
	}

	modelWallFlont_->Draw(worldWallBack, &camera, &color_);
	modelWallFlont_->Draw(worldWallLeft, &camera, &color_);
	modelWallFlont_->Draw(worldWallRight, &camera, &color_);
}