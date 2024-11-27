#pragma once

#include "WorldTransform.h"
#include "ObjectModel.h"
#include "Camera.h"

#include <memory>
#include <string>

class ResultEnemy
{
public:

    ResultEnemy() = default;
    ~ResultEnemy() = default;

    void Initialize();
    void Update();
    void Draw(const Camera* _camera);

private:
    void Scaling();

    std::unique_ptr <ObjectModel> model_ = nullptr;

    Vector3 baseScale_ = { 1,1,1 };

    float currentTime_ = 0.0f;

    Vector3 scaleRange_ = { 1.2f,1.2f ,1.2f };

    float scaleSpeed_ = 0.1f;

    std::string modelName_ = "boss/boss.obj";




#ifdef _DEBUG
    void ImGui();
#endif // _DEBUG

};