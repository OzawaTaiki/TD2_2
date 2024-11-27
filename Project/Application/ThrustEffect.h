#pragma once

#include "Matrix4x4.h"
#include "ParticleEmitters.h"
#include <memory>

class ThrustEffect
{
public:
    ThrustEffect() = default;
    ~ThrustEffect() = default;

    void Initialize(const Matrix4x4* _worldMatrix);
    void Update();
    void Draw();

    void Start();

private:
    std::unique_ptr<ParticleEmitter> thrust_;
    std::unique_ptr<ParticleEmitter> thrust2_;

    bool isEmit_ = false;

    float timer_ = 0.0f;
    float timerMax_ = 1.0f;
};
