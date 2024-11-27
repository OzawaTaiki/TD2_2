#include "EnemyDeathParticle.h"
#include "ParticleManager.h"
#include "TextureManager.h"

void EnemyDeathParticle::Initialize(const std::string& _name)
{
    emitter_ = std::make_unique<ParticleEmitter>();
    emitter_->SetShape_Box(Vector3{ 1,1,1 });

    emitter_->Setting(_name);

    uint32_t texture = TextureManager::GetInstance()->Load("circle.png");
    ParticleManager::GetInstance()->CreateParticleGroup(emitter_->GetName(), "plane/plane.obj", emitter_.get(), texture);
}

void EnemyDeathParticle::Update(bool _move)
{
    assert(playerMat_ != nullptr);

    emitter_->SetCenter(playerMat_->GetWorldPosition());

    emitter_->SetEmit(_move);
    emitter_->Update();
}

void EnemyDeathParticle::Draw()
{
    emitter_->Draw();
}
