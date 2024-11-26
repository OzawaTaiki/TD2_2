#pragma once
#include "BaseScene.h"
#include <memory>

class GameOverScene : public BaseScene
{
public:

    static std::unique_ptr<BaseScene> Create();

     ~GameOverScene() override;

    void Initialize() override;
    void Update() override;
    void Draw() override;

private:

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};