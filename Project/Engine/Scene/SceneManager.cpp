#include "SceneManager.h"

#include <cassert>

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
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

}

void SceneManager::Update()
{
    currentScene_->Update();
}

void SceneManager::Draw()
{
    currentScene_->Draw();

    if (nextSceneName_ != "empty")
    {
        ChangeScene();
    }
}

void SceneManager::ReserveScene(const std::string& _name)
{
    auto instance = GetInstance();

    assert(instance->scenes_.find(_name) != instance->scenes_.end());
    instance->nextSceneName_ = _name;
}

void SceneManager::ChangeScene()
{
    currentScene_ = scenes_[nextSceneName_]();
    currentScene_->Initialize();
    currentSceneName_ = nextSceneName_;
    nextSceneName_ = "empty";
}
