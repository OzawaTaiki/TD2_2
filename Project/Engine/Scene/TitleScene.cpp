#include "TitleScene.h"

#include "SceneManager.h"
#include "Input.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "ConfigManager.h"

std::unique_ptr<BaseScene>TitleScene::Create()
{
	return std::make_unique<TitleScene>();
}

TitleScene::~TitleScene()
{

}

void TitleScene::Initialize()
{
	player_ = std::make_unique<TItlePlayer>();
	player_->Initialize();

	enemy_ = std::make_unique<TitleEnemy>();
	enemy_->Initialize();

	camera_.Initialize();
	camera_.translate_ = { 0,15,-50 };
	camera_.rotate_ = { 0.26f,0,0 };

	UIs_ = std::make_unique<TitleUI>();
	UIs_->Initialize();

	ConfigManager::GetInstance()->SetVariable("Title", "backGroundTexturePath", &backGroundTexturePath_);

	uint32_t handle = TextureManager::GetInstance()->Load(backGroundTexturePath_);

	backGround_ = Sprite::Create(handle, { 0,0 });
	backGround_->Initialize();
	backGround_->SetSize({ 1280,720 });

	//音
	audio_ = std::make_unique<Audio>();
	audio_->Initialize();
	audio2_ = std::make_unique<Audio>();
	audio2_->Initialize();

	// 移動音
	titleBgm_.soundDataHandle = audio_->SoundLoadWave("resources/Sounds/titleBgm.wav");
	titleBgm_.volume = 0.2f;
	titleBgm_.voiceHandle = audio_->SoundPlay(titleBgm_.soundDataHandle, titleBgm_.volume, 1, 0);

	titleSpace_.soundDataHandle = audio2_->SoundLoadWave("resources/Sounds/titleSpace.wav");
	titleSpace_.volume = 0.2f;
	
}

void TitleScene::Update()
{
	camera_.Update();

	player_->Update();
	enemy_->Update();

	UIs_->Update();

	camera_.UpdateMatrix();

	
	if (Input::GetInstance()->IsKeyTriggered(DIK_SPACE) ||
		Input::GetInstance()->IsPadTriggered(PadButton::iPad_A))
	{
		titleSpace_.voiceHandle = audio2_->SoundPlay(titleSpace_.soundDataHandle, titleSpace_.volume, 1, 0);
		audio_->SoundStop(titleBgm_.voiceHandle);
		SceneManager::ReserveScene("game");
	}
	
#ifdef _DEBUG
	ImGui();
#endif // _DEBUG
}

void TitleScene::Draw()
{
	Sprite::PreDraw();
	backGround_->Draw();

	ModelManager::GetInstance()->PreDrawForObjectModel();

	player_->Draw(&camera_);
	enemy_->Draw(&camera_);

	UIs_->Draw(&camera_);
}

#ifdef _DEBUG
#include <imgui.h>
void TitleScene::ImGui()
{
	char buf[255];
	strcpy_s(buf, backGroundTexturePath_.c_str());
	if (ImGui::InputText("backGroundTexturePath", buf, sizeof(buf)))
	{
		backGroundTexturePath_ = buf;
	}
	if (ImGui::Button("save"))
	{
		ConfigManager::GetInstance()->SaveData("Title");
		backGround_->SetTextureHandle(TextureManager::GetInstance()->Load(backGroundTexturePath_));
		UIs_->Save();
	}

}
#endif // _DEBUG