#pragma once
#include "BaseScene.h"
#include <memory>

class GameClearScene : public BaseScene
{
public:

    static std::unique_ptr<BaseScene> Create();

     ~GameClearScene() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;

private:

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};