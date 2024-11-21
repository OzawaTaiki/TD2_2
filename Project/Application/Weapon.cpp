#include "Weapon.h"
#include "../Collider/CollisionManager.h"
#include "ParticleManager.h"
#include "TextureManager.h"

void Weapon::Initialize()
{
	worldTransform_.Initialize();
	model_ = Model::CreateFromObj("Sword/Sword.obj");

	color_.Initialize();
	color_.SetColor(Vector4{ 1, 1, 1, 1 });

    collider_ = std::make_unique<Collider>();
	collider_->SetBoundingBox(Collider::BoundingBox::OBB_3D);
	collider_->SetShape(model_->GetMin(), model_->GetMax());
    collider_->SetAtrribute("weapon");
	collider_->SetMask({ "player","weapon" });
    collider_->SetGetWorldMatrixFunc([this]() { return worldTransform_.matWorld_; });
	collider_->SetOnCollisionFunc([this](const Collider* _other) {OnCollision(_other); });

	uint32_t texture = TextureManager::GetInstance()->Load("white.png");
    hitPatricles_ = std::make_unique<ParticleEmitter>();
	hitPatricles_->Setting("HitParticle");
	hitPatricles_->SetWorldMatrix(&worldTransform_.matWorld_);
	ParticleManager::GetInstance()->CreateParticleGroup(hitPatricles_->GetName(), "sphere/sphere.obj", hitPatricles_.get(),texture);

    hitPatricles2_ = std::make_unique<ParticleEmitter>();
    hitPatricles2_->Setting("HitParticle2");
    hitPatricles2_->SetWorldMatrix(&worldTransform_.matWorld_);
    ParticleManager::GetInstance()->CreateParticleGroup(hitPatricles2_->GetName(), "sphere/sphere.obj", hitPatricles2_.get(), texture);

}

void Weapon::UpdateWorldTransform()
{
	worldTransform_.UpdateData();

	hitPatricles_->Update();
    hitPatricles2_->Update();
     
}

void Weapon::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);

	hitPatricles_->Draw();
    hitPatricles2_->Draw();
    collider_->Draw();
}

void Weapon::RegisterCollider()
{
	CollisionManager::GetInstance()->RegisterCollider(collider_.get());
}

void Weapon::OnCollision(const Collider* _other)
{
    std::string name = _other->GetName();

    if (name == "enemy")
    {
        // プレイヤーの攻撃が当たった瞬間
		// エフェクト出す
        if (collider_->IsCollisionEnter())
        {
            hitPatricles_->SetEmit(true);
            hitPatricles2_->SetEmit(true);
        }
    }
}
