#pragma once

#include "Matrix4x4.h"
#include "ParticleEmitters.h"

#include <memory>
#include <string>

class HitEffect
{
public:
    HitEffect()=default;
    ~HitEffect()= default;

    void Initilize(const std::string& _name ,const Matrix4x4* _worldMatrix);
    void Update();
    void Draw();

    void SetEmit(bool _emit);

private:
    std::unique_ptr<ParticleEmitter> hitPatricles_;
    std::unique_ptr<ParticleEmitter> hitPatricles2_;

};