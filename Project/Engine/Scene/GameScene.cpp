#include "GameScene.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "ParticleManager.h"
#include "TextureManager.h"
#include "ConfigManager.h"
#include "Quaternion.h"
#include "MyLib.h"
#include <chrono>
#include <imgui.h>

GameScene::~GameScene()
{
    delete color_;
}

void GameScene::Initialize()
{
    input_ = Input::GetInstance();

    camera_ = std::make_unique<Camera>();
    camera_->Initialize();

    lineDrawer_ = LineDrawer::GetInstance();
    lineDrawer_->SetCameraPtr(camera_.get());


    audio_ = std::make_unique<Audio>();
    audio_->Initialize();

    model_ = Model::CreateFromObj("bunny.gltf");
    trans_.Initialize();
    trans_.UpdateData();
    color_ = new ObjectColor;
    color_->Initialize();

}

void GameScene::Update()
{
    //ImGui::ShowDemoWindow();
    ImGui::Begin("Engine");

    input_->Update();
    //<-----------------------
    camera_->Update();

    Quaternion rotation0 = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(.71f, .71f, 0), 0.3f);
    Quaternion rotation1 = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(.71f, 0, .71f), 3.141592f);

    Quaternion interpolat0 = Slerp(rotation0, rotation1, 0.0f);
    Quaternion interpolat1 = Slerp(rotation0, rotation1, 0.3f);
    Quaternion interpolat2 = Slerp(rotation0, rotation1, 0.5f);
    Quaternion interpolat3 = Slerp(rotation0, rotation1, 0.7f);
    Quaternion interpolat4 = Slerp(rotation0, rotation1, 1);

    interpolat0.ShowData("interpolat0", true);
    interpolat1.ShowData("interpolat1", true);
    interpolat2.ShowData("interpolat2", true);
    interpolat3.ShowData("interpolat3", true);
    interpolat4.ShowData("interpolat4", true);

    trans_.UpdateData();

    camera_->UpdateMatrix();
    //<-----------------------
    ImGui::End();
}

void GameScene::Draw()
{
    ModelManager::GetInstance()->PreDraw();
    //<------------------------
    model_->Draw(trans_, camera_.get(), color_);

    //<------------------------


    Sprite::PreDraw();
    //<------------------------



    //<------------------------
    lineDrawer_->Draw();


}
