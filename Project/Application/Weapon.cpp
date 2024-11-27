#include "Weapon.h"
#include "../Collider/CollisionManager.h"
#include "ParticleManager.h"

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

    hitPatricles_ = std::make_unique<HitEffect>();
    hitPatricles_->Initilize("weapon", &worldTransform_.matWorld_);

    slashModel_ = std::make_unique<ObjectModel>();
    slashModel_->Initialize("Slash/Slash_test.obj");
    slashModel_->SetParent(&worldTransform_);
    slashModel_->translate_ = { 0,0,2 };
    slashModel_->scale_= { 2,2,2 };




}

void Weapon::UpdateWorldTransform()
{
  	worldTransform_.UpdateData();
	hitPatricles_->Update();

    if (SlashUpdate_)
    {
        slashModel_->Update();
        SlashUpdate_ = false;
    }
}

void Weapon::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, &camera, &color_);

    if(isSlashEffect_)
        slashModel_->Draw(&camera, Vector4{ 1,1,1,1 });

	hitPatricles_->Draw();

#ifdef _DEBUG
    collider_->Draw();
#endif // _DEBUG

}

void Weapon::StartSlashEffect()
{
    isSlashEffect_ = true;
    SlashUpdate_ = true;
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
        }
    }
}

void Weapon::SetLight(LightGroup* _ptr)
{
    model_->SetLightGroup(_ptr);
}
