#include "WinApp.h"
#include "DXCommon.h"
#include "Input.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "GameScene.h"
#include "LineDrawer.h"
#include "SRVManager.h"
#include "ImGuiManager.h"
#include "ParticleManager.h"
#include "RandomGenerator.h"
#include "ConfigManager.h"
#include "TitleScene.h"
#include "SceneManager.h"
#include <random>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WinApp* winApp = WinApp::GetInstance();
	winApp->Initilize();

	DXCommon* dxCommon =  DXCommon::GetInstance();
	dxCommon->Initialize(winApp,WinApp::kWindowWidth_, WinApp::kWindowHeight_);

	SRVManager* srvManager = SRVManager::GetInstance();
	srvManager->Initialize();
	PSOManager::GetInstance()->Initialize();


	std::unique_ptr<ImGuiManager> imguiManager = std::make_unique <ImGuiManager>();
	imguiManager->Initialize();

	ParticleManager* particle = ParticleManager::GetInstance();
	particle->Initialize();

	ConfigManager::GetInstance()->Initialize();

	TextureManager::GetInstance()->Initialize();
	TextureManager::GetInstance()->Load("cube.jpg");
	TextureManager::GetInstance()->Load("uvChecker.png");

	Sprite::StaticInitialize(WinApp::kWindowWidth_, WinApp::kWindowHeight_);
	ModelManager::GetInstance()->Initialize();

	LineDrawer* lineDrawer = LineDrawer::GetInstance();
	lineDrawer->Initialize();

	Input* input = Input::GetInstance();
	input->Initilize(winApp);


	SceneManager::RegisterScene("title", TitleScene::Create);
	SceneManager::RegisterScene("game", GameScene::Create);
	SceneManager::GetInstance()->Initialize("game");

	///
	/// メインループ
	///
	// ウィンドウのｘボタンが押されるまでループ
	while (!winApp->ProcessMessage())
	{
		imguiManager->Begin();

		///
		/// 更新処理ここから
		///

        SceneManager::GetInstance()->Update();
		//gameScene->Update();

		///
		/// 更新処理ここまで
		///

		dxCommon->PreDraw();
		srvManager->PreDraw();

		///
		/// 描画ここから
		///

		SceneManager::GetInstance()->Draw();
		//gameScene->Draw();


		///
		/// 描画ここまで
		///
		imguiManager->End();
		imguiManager->Draw();

		dxCommon->PostDraw();

		SceneManager::ChangeScene();
	}
	imguiManager->Finalize();

	winApp->Filalze();


	return 0;
}

