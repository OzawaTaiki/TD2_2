#pragma once
#include "BaseScene.h"
#include <memory>

#include "ResultPlayer.h"
#include "ResultUI.h"
#include "Audio.h"

class GameClearScene : public BaseScene
{
public:

    static std::unique_ptr<BaseScene> Create();

     ~GameClearScene() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;


private:
    Camera camera_ = {};
    std::unique_ptr<ResultPlayer> player_ = nullptr;
    ResultUI ui_ = {};
    std::unique_ptr<Audio> audio_;
     std::unique_ptr<Audio> audio2_;
     
     std::unique_ptr<LightGroup> lightGroup_ = nullptr;

    // 音
    struct Sound {
        uint32_t soundDataHandle;
        uint32_t voiceHandle;
        float volume;
    };
    Sound clearBgm_;
    Sound decision;
#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};