#pragma once

#include "ObjectModel.h"
#include "Sprite.h"

#include <memory>

class TitleUI
{
public:

    TitleUI() = default;
    ~TitleUI() = default;

    void Initialize();
    void Update();
    void Draw(const Camera* _camera);

    void Save();

private:

    void TextUpdate();
    //void ChangeTexture(const std::);

    std::unique_ptr<ObjectModel> title_ = nullptr;
    std::string titleModelpath_ = "cube/cube.obj";

    Sprite* pressAnyKey_ = nullptr;
    Vector4 pressTextColor_ = { 1,1,1,1 };
    std::string textTextureParh_ = "cube.jpg";

    // テキストの点滅
    // 現在の時間
    float currentTime_ = 0.0f;
    // 周期
    float cycle_ = 3.0f;
    // 閾値
    float threshold_ = 0.9f;


};