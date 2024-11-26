#pragma once
#include "Camera.h"
#include "ParticleEmitters.h"
#include "WorldTransform.h"

#include <memory>

class EnemyDeathParticle
{
public:
    EnemyDeathParticle() = default;
    ~EnemyDeathParticle() = default;

    void Initialize(const std::string& _name);
    void Update(bool _move);
    void Draw();

    void SetPlayerMat(const WorldTransform* _playerMat) { playerMat_ = _playerMat; }

private:

    const WorldTransform* playerMat_ = nullptr;

    std::unique_ptr <ParticleEmitter> emitter_ = nullptr;



};

