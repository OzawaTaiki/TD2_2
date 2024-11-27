#pragma once
#include "BaseScene.h"
#include "Audio.h"
#include <memory>

#include "Enemy/ResultEnemy.h"
#include "ResultUI.h"


class GameOverScene : public BaseScene
{
public:

    static std::unique_ptr<BaseScene> Create();

    ~GameOverScene() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;


private:
    Camera camera_ = {};
    std::unique_ptr<ResultEnemy> enemy_ = nullptr;
    ResultUI ui_ = {};
    std::unique_ptr<Audio> audio_;
    std::unique_ptr<Audio> audio2_;
    // 音
    struct Sound {
        uint32_t soundDataHandle;
        uint32_t voiceHandle;
        float volume;
    };
    Sound gameOverBgm_;
    Sound decision;

    
#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};