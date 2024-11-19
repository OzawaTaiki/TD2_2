#pragma once

#include "Camera.h"
#include "ParticleEmitters.h"
#include "WorldTransform.h"

#include <memory>

class PlayerDustParticle
{
public:
    PlayerDustParticle() = default;
    ~PlayerDustParticle() = default;

    void Initialize();
    void Update(bool _move);
    void Draw();

    void SetPlayerMat(const WorldTransform* _playerMat) { playerMat_ = _playerMat; }

private:

    const WorldTransform* playerMat_ = nullptr;

    std::unique_ptr <ParticleEmitter> emitter_ = nullptr;



};