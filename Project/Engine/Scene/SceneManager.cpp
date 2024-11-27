#include "SceneManager.h"
#include "Input.h"
#include "TextureManager.h"
#include <cassert>

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

SceneManager::~SceneManager()
{
    scenes_.clear();
    currentScene_.reset();
}

void SceneManager::RegisterScene(const std::string& _name, SceneFactory _scene)
{
    auto instance = GetInstance();
    instance->scenes_[_name] = _scene;
}

void SceneManager::Initialize(const std::string& _name)
{
    assert(scenes_.size() > 0);
    auto it = scenes_.find(_name);
    assert(it != scenes_.end());

    currentScene_ = it->second();
    currentSceneName_ = _name;

    nextSceneName_ = "empty";
    currentScene_->Initialize();

    uint32_t handle = TextureManager::GetInstance()->Load("white.png");
    fadeSprite_ = Sprite::Create(handle, { 0,0 });
    fadeSprite_->Initialize();
    fadeSprite_->SetSize({ 1280, 720 });
    fadeAlphaSpeed_ = 0.02f;



}

void SceneManager::Update()
{
#ifdef _DEBUG
    ImGui();
#endif // _DEBUG
    Input::GetInstance()->Update();
    currentScene_->Update();
}

void SceneManager::Draw()
{
    currentScene_->Draw();
    Fade();
}

void SceneManager::ReserveScene(const std::string& _name)
{
    auto instance = GetInstance();

    assert(instance->scenes_.find(_name) != instance->scenes_.end());
    if (instance->currentSceneName_ == _name || instance->isChangingScene_)
        return;

    instance->nextSceneName_ = _name;

    instance->FadeStart();
}

void SceneManager::ChangeScene()
{
    auto instance = GetInstance();

    if (instance->isChangingScene_)
    {
        return;
    }

    if (instance->nextSceneName_ == "empty" ||
        instance->currentSceneName_ == instance->nextSceneName_)
    {
        return;
    }

    instance->currentScene_.reset();

    instance->currentScene_ = instance->scenes_[instance->nextSceneName_]();
    instance->currentScene_->Initialize();
    instance->currentSceneName_ = instance->nextSceneName_;
    instance->nextSceneName_ = "empty";
    instance->FadeStart();
}

void SceneManager::Fade()
{
    if (isChangingScene_)
    {
        fadeAlpha_ += fadeAlphaSpeed_;
        if (fadeAlpha_ > 1.0f)
        {
            fadeAlpha_ = 1.0f;
            FadeEnd();
        }
        else if (fadeAlpha_ < 0.0f)
        {
            fadeAlpha_ = 0.0f;
            FadeEnd();
        }
        Sprite::PreDraw();
        fadeSprite_->Draw({ 0,0,0,fadeAlpha_ });
    }
}


void SceneManager::FadeStart()
{
    if (fadeAlpha_ == 0.0f)
        fadeAlphaSpeed_ = 0.02f;
    else if (fadeAlpha_ == 1.0f)
        fadeAlphaSpeed_ = -0.02f;
    else
        return;
    isChangingScene_ = true;
}


void SceneManager::FadeEnd()
{
    if (fadeAlpha_ == 1.0f)
        fadeAlphaSpeed_ = -0.02f;
    else if (fadeAlpha_ == 0.0f)
        fadeAlphaSpeed_ = 0.02f;
    else
        return;
    isChangingScene_ = false;
}


#ifdef _DEBUG
#include <imgui.h>
void SceneManager::ImGui()
{
    char comboLabel[128];

    ImGui::Begin("SceneManager");
    for (auto& scene : scenes_)
    {
        strcpy_s(comboLabel, scene.first.c_str());
        if (ImGui::Button(comboLabel))
        {
            ReserveScene(scene.first);
            break;
        }
    }

    ImGui::Text("Current Scene : %s", currentSceneName_.c_str());


    ImGui::End();
}
#endif // _DEBUG
