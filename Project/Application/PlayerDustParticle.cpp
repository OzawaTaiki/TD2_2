#include "PlayerDustParticle.h"
#include "ParticleManager.h"
#include "TextureManager.h"

void PlayerDustParticle::Initialize()
{
    emitter_ = std::make_unique<ParticleEmitter>();
    emitter_->SetShape_Box(Vector3{ 1,1,1 });
    //emitter_->SetName("PlayerDust");
    /*emitter_->Setting(Vector3{ 0,0,0 },
                      Vector3{ 0,0,0 },
                      3,
                      1,
                      10,
                      false);*/
    emitter_->Setting("PlayerDust");

    uint32_t texture = TextureManager::GetInstance()->Load("circle.png");
    ParticleManager::GetInstance()->CreateParticleGroup(emitter_->GetName(), "plane/plane.obj", emitter_.get(), texture);
}

void PlayerDustParticle::Update(bool _move)
{
    assert(playerMat_ != nullptr);

    emitter_->SetCenter(playerMat_->GetWorldPosition());

    emitter_->SetEmit(_move);
    emitter_->Update();
}

void PlayerDustParticle::Draw()
{
    emitter_->Draw();
}
