#pragma once
#include "BaseScene.h"
#include <memory>

#include "ResultPlayer.h"
#include "ResultUI.h"

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

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};