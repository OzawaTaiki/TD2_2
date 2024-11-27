#pragma once

#include "Sprite.h"

#include <cstdint>
#include <memory>
#include <string>

class HPUISprite;
class GameSceneUI
{
public:
    GameSceneUI() = default;
    ~GameSceneUI() = default;

    void Initialize();
    void Update(float _playerHP, float _enemyHP);
    void Draw();

private:

    std::unique_ptr <HPUISprite> playerHP_ = nullptr;
    std::unique_ptr <HPUISprite> enemyHP_ = nullptr;

    Sprite* Controler_ = nullptr;

};

class HPUISprite
{
public:
    HPUISprite() = default;
    ~HPUISprite() = default;

    void Initialize(uint32_t _frameHandle, uint32_t _baseHandle, uint32_t _barHandle, const std::string& _name);
    void Update(float _HP );
    void Draw();

private:
    Sprite* frame_ = nullptr;
    Sprite* base_ = nullptr;
    Sprite* hpBar_ = nullptr;

    Vector2 basePos_ = {};
    Vector2 baseSize_ = {};

    Vector2 barOffset_ = {};
    Vector2 barSize_ = {};

    std::string name_ = "";
};
