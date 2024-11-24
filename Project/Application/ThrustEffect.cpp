#include "ThrustEffect.h"
#include "ParticleManager.h"

void ThrustEffect::Initialize(const Matrix4x4* _worldMatrix)
{
    thrust_ = std::make_unique<ParticleEmitter>();
    thrust_->Setting("thrust1");
    thrust_->SetWorldMatrix(_worldMatrix);

    thrust2_ = std::make_unique<ParticleEmitter>();
    thrust2_->Setting("thrust2");
    thrust2_->SetWorldMatrix(_worldMatrix);


    ParticleManager::GetInstance()->CreateParticleGroup(thrust_->GetName(), "plane/plane.obj", thrust_.get(),0);
    ParticleManager::GetInstance()->CreateParticleGroup(thrust2_->GetName(), "plane/plane.obj", thrust2_.get(), 0);

}

void ThrustEffect::Update()
{

    thrust_->Update();
    thrust2_->Update();
    if (!isEmit_)
        return;

    timer_ += 1.0f / 60.0f;
    if (timer_ > timerMax_)
    {
        isEmit_ = false;
        timer_ = 0.0f;
    }


}

void ThrustEffect::Draw()
{
    if (!isEmit_)
        return;

    thrust_->Draw();
    thrust2_->Draw();
}

void ThrustEffect::Start()
{
    thrust_->SetEmit(1);
    thrust2_->SetEmit(1);
}
