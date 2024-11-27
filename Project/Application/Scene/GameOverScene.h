#pragma once
#include "BaseScene.h"
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

#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG
};