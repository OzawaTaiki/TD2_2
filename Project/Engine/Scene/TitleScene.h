#pragma once
#include "BaseScene.h"
#include <memory>
#include <string>
#include "Audio.h"

#include "TItlePlayer.h"
#include "Enemy/TitleEnemy.h"
#include "TItleUI.h"
#include "Camera.h"

class TitleScene : public BaseScene
{
public:

    static std::unique_ptr<BaseScene> Create();

     ~TitleScene() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;

private:



    std::unique_ptr<TItlePlayer> player_ = nullptr;
    std::unique_ptr<TitleEnemy> enemy_ = nullptr;

    std::unique_ptr<TitleUI> UIs_ = nullptr;

    Sprite* backGround_ = nullptr;
    std::string backGroundTexturePath_ = "white.png";


    Camera camera_;

    std::unique_ptr<Audio> audio_;
     std::unique_ptr<Audio> audio2_;

     std::unique_ptr <LightGroup> lightGroup_ = nullptr;

   
    // 音
    struct Sound {
        uint32_t soundDataHandle;
        uint32_t voiceHandle;
        float volume;
    };
    Sound titleBgm_;
    Sound titleSpace_;

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};