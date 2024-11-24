#include "HitEffect.h"
#include "ParticleManager.h"
#include "TextureManager.h"
void HitEffect::Initilize(const std::string& _name, const Matrix4x4* _worldMatrix)
{
    uint32_t texture = TextureManager::GetInstance()->Load("white.png");
    hitPatricles_ = std::make_unique<ParticleEmitter>();
    hitPatricles_->Setting(_name + "HitParticle");
    hitPatricles_->SetWorldMatrix(_worldMatrix);
    ParticleManager::GetInstance()->CreateParticleGroup(hitPatricles_->GetName(), "sphere/sphere.obj", hitPatricles_.get(), texture);
    hitPatricles2_ = std::make_unique<ParticleEmitter>();
    hitPatricles2_->Setting(_name + "HitParticle2");
    hitPatricles2_->SetWorldMatrix(_worldMatrix);
    ParticleManager::GetInstance()->CreateParticleGroup(hitPatricles2_->GetName(), "sphere/sphere.obj", hitPatricles2_.get(), texture);
}

void HitEffect::Update()
{
    hitPatricles_->Update();
    hitPatricles2_->Update();
}

void HitEffect::Draw()
{
    hitPatricles_->Draw();
    hitPatricles2_->Draw();
}

void HitEffect::SetEmit(bool _emit)
{
    hitPatricles_->SetEmit(_emit);
    hitPatricles2_->SetEmit(_emit);
}
