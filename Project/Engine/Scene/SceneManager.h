#pragma once
#include "BaseScene.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

using SceneFactory = std::unique_ptr<BaseScene>(*)();

class SceneManager
{
public:
    static SceneManager* GetInstance();

    // �V�[���̓o�^
    // _name : �V�[���̖��O
    // _scene : �V�[���̐����֐�
    // �� : SceneManager::RegisterScene("game", Game::Create);
    static void RegisterScene(const std::string& _name, SceneFactory _scene);

    // ������
    // _name : ����������V�[���̖��O
    void Initialize(const std::string& _name);
    // �X�V
    void Update();
    // �`��
    void Draw();

    // �V�[���̗\��
    // _name : �\�񂷂�V�[���̖��O
    static void ReserveScene(const std::string& _name);

private:
    // �V�[���̕ύX
    void ChangeScene();

    // �V�[���̃��X�g
    std::unordered_map<std::string, SceneFactory>   scenes_ = {};
    // ���݂̃V�[��
    std::unique_ptr<BaseScene> currentScene_ = nullptr;

    // ���݂̃V�[����
    std::string currentSceneName_ = {};
    // ���̃V�[����
    std::string nextSceneName_ = {};

};