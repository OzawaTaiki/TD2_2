#include "Stage.h"
#include "../Enemy/Enemy.h"

void Stage::Initialize()
{
	worldSkyBox.Initialize();
	worldSkyBox.transform_ = { 0,0,0 };
	worldSkyBox.scale_ = { 3,3,3 };

	colorSky_.Initialize();
	colorSky_.SetColor(Vector4(0.77f, 0.77f, 0.77f, 1.0f));

	modelSkyBox_ = Model::CreateFromObj("SkyBox/SkyBox.obj");
	// 床
	worldFloor_.Initialize();
	worldFloor_.transform_ = Vector3{ 0,-3.0f,0 };
	worldFloor_.rotate_.y = { 1.57f };
	worldFloor_.scale_ = { 0.55f,0.55f,0.55f };
	// 床モデル
	modelFloor_ = Model::CreateFromObj("Tile/Tile.gltf");

	// 壁(前)
	worldWallFlont.Initialize();
	worldWallFlont.transform_ = Vector3{ 0,0,-100 };
	worldWallFlont.rotate_ = Vector3{ 0,0,0 };

	worldWallBack.Initialize();
	worldWallBack.transform_ = Vector3{ 0,0,100 };
	worldWallBack.rotate_ = Vector3{ 0,-3.14f,0 };

	worldWallRight.Initialize();
	worldWallRight.transform_ = Vector3{ 100,0,0 };
	worldWallRight.rotate_ = Vector3{ 0,DegreesToRadians(-90),0 };

	worldWallLeft.Initialize();
	worldWallLeft.transform_ = Vector3{ -100,0,0 };
	worldWallLeft.rotate_ = Vector3{ 0,DegreesToRadians(90),0 };

	modelWallFlont_ = Model::CreateFromObj("wall/wall.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

    ConfigManager* configManager = ConfigManager::GetInstance();

	configManager->SetVariable("stage", "worldFloor_translate", &worldFloor_.transform_);
	configManager->SetVariable("stage", "worldFloor_rotate", &worldFloor_.rotate_);
	configManager->SetVariable("stage", "worldWallFlont_translate", &worldWallFlont.transform_);
	configManager->SetVariable("stage", "worldWallFlont_rotate", &worldWallFlont.rotate_);
	configManager->SetVariable("stage", "worldWallBack_translate", &worldWallBack.transform_);
	configManager->SetVariable("stage", "worldWallBack_rotate", &worldWallBack.rotate_);
	configManager->SetVariable("stage", "worldWallLeft_translate", &worldWallLeft.transform_);
	configManager->SetVariable("stage", "worldWallLeft_rotate", &worldWallLeft.rotate_);
	configManager->SetVariable("stage", "worldWallRight_translate", &worldWallRight.transform_);
	configManager->SetVariable("stage", "worldWallRight_rotate", &worldWallRight.rotate_);


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

	worldSkyBox.UpdateData();

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

	bool isNotAttackBehavior = (enemy_->GetBehavior() != Enemy::Behavior::kAttack);
	bool isNotSpecialAttackBehavior = (enemy_->GetattackBehavior() != Enemy::AttackBehavior::kSpecial);
	bool isNotSpecialAttack2 = (enemy_->GetSpecialAttack() != Enemy::SpecialAttack::kAttack2);

	if (isNotAttackBehavior || (isNotSpecialAttackBehavior || isNotSpecialAttack2)) {

		modelWallFlont_->Draw(worldWallFlont, &camera, &color_);
	}
	else {
		worldWallFlont.UpdateData();
	}

	modelWallFlont_->Draw(worldWallBack, &camera, &color_);
	modelWallFlont_->Draw(worldWallLeft, &camera, &color_);
	modelWallFlont_->Draw(worldWallRight, &camera, &color_);

	modelSkyBox_->Draw(worldSkyBox, &camera, &colorSky_);
}