#include "ParticleEmitters.h"
#include "ParticleManager.h"
#include "RandomGenerator.h"
#include "LineDrawer.h"
#include "MatrixFunction.h"
#include "ConfigManager.h"
#include "VectorFunction.h"
#include <imgui.h>

void ParticleEmitter::Setting(const Vector3& _center, const Vector3& _rotate, uint32_t _countPerEmit, uint32_t _emitPerSec, uint32_t _maxParticle, bool _randomColor)
{
    position_ = _center;
    rotate_ = _rotate;
    countPerEmit_ = _countPerEmit;
    emitPerSec_ = _emitPerSec;
    maxParticles_ = _maxParticle;
    randomColor_ = _randomColor;

    setting_.acceleration = { {0,0,0},{0,0,0} };
    setting_.color = { {1,1,1,1},{1,1,1,1} };
    setting_.direction = { {-1,-1,-1},{1,1,1} };
    setting_.lifeTime = { 1,3 };
    setting_.rotate = { {0,0,0} ,{0,0,0} };
    setting_.size = { {1,1,1},{1,1,1} };
    setting_.spped = { 0.1f,0.5f };

    emitTime_ = 1.0f / static_cast<float> (emitPerSec_);
}

void ParticleEmitter::Setting(const std::string& _name)
{
    name_ = _name;

    ConfigManager* instance = ConfigManager::GetInstance();

    instance->SetVariable(name_, "lifeTime_min", &setting_.lifeTime.min);
    instance->SetVariable(name_, "lifeTime_max", &setting_.lifeTime.max);
    instance->SetVariable(name_, "size_min", &setting_.size.min);
    instance->SetVariable(name_, "size_max", &setting_.size.max);
    instance->SetVariable(name_, "rotate_min", &setting_.rotate.min);
    instance->SetVariable(name_, "rotate_max", &setting_.rotate.max);
    instance->SetVariable(name_, "spped_min", &setting_.spped.min);
    instance->SetVariable(name_, "spped_max", &setting_.spped.max);
    instance->SetVariable(name_, "direction_min", &setting_.direction.min);
    instance->SetVariable(name_, "direction_max", &setting_.direction.max);
    instance->SetVariable(name_, "acceleration_min", &setting_.acceleration.min);
    instance->SetVariable(name_, "acceleration_max", &setting_.acceleration.max);
    instance->SetVariable(name_, "color_min", &setting_.color.min);
    instance->SetVariable(name_, "color_max", &setting_.color.max);

    instance->SetVariable(name_, "countPerEmit", &countPerEmit_);
    instance->SetVariable(name_, "emitPerSec", &emitPerSec_);
    instance->SetVariable(name_, "maxParticles", &maxParticles_);
    instance->SetVariable(name_, "randomColor", reinterpret_cast<uint32_t*> (&randomColor_));

    instance->SetVariable(name_, "shape", reinterpret_cast<uint32_t*>(&shape_));
    instance->SetVariable(name_, "size", &size_);
    instance->SetVariable(name_, "radius", &radius_);
    instance->SetVariable(name_, "position", &position_);
    instance->SetVariable(name_, "rotate", &rotate_);
    instance->SetVariable(name_, "offset", &offset_);
    emitTime_ = 1.0f / static_cast<float> (emitPerSec_);

    switch (shape_)
    {
    case EmitterShape::Box:
        SetShape_Box(size_);
        break;
    case EmitterShape::Shpere:
        SetShape_Sphere(radius_);
        break;
    case EmitterShape::Circle:
        SetShape_Circle(radius_);
        break;
    case EmitterShape::None:
        break;
    default:
        break;
    }
}

void ParticleEmitter::Update()
{
    if (parentMatWorld_)
        position_ = Transform(offset_, *parentMatWorld_);
    else
        position_ = position_+offset_;

    currentTime_ += deltaTime_;
    if (!emit_) {
        currentTime_ = 0;
    }
    ImGui::Begin("emit");
    static const char* combo[1024] = { "Box","Sphere","Circle","None" };

    if (emitTime_ <= currentTime_)
    {
        std::vector<Particle> particles;

        for (uint32_t count = 0; count < countPerEmit_; ++count)
        {
            particles.push_back(GenerateParticleData());
        }

        ParticleManager::GetInstance()->AddParticleToGroup(name_, particles);
        currentTime_ = 0;
    }

    ImGui::BeginTabBar("setting");
    if (ImGui::BeginTabItem(name_.c_str()))
    {
        ImGui::Combo("shape", reinterpret_cast<int*>(&shape_), combo, 4);

        ImGui::SeparatorText("Emitter");
        ImGui::DragFloat3("size", &size_.x,0.01f);
        ImGui::DragFloat("radius", &radius_, 0.01f);
        ImGui::DragFloat3("offset", &offset_.x, 0.01f);
        ImGui::DragInt("countPerEmit", reinterpret_cast<int*>(&countPerEmit_));
        ImGui::DragInt("emitPerSec", reinterpret_cast<int*>(&emitPerSec_));
        ImGui::DragInt("maxParticles", reinterpret_cast<int*>(&maxParticles_));
        ImGui::Checkbox("randomColor", &randomColor_);

        ImGui::Spacing();

        ImGui::SeparatorText("Particle_Init");
        ImGui::DragFloatRange2("lifeTime", &setting_.lifeTime.min, &setting_.lifeTime.max);
        ImGui::DragFloat3("size_min", &setting_.size.min.x);
        ImGui::DragFloat3("size_max", &setting_.size.max.x);
        ImGui::DragFloat3("rotate_min", &setting_.rotate.min.x);
        ImGui::DragFloat3("rotate_max", &setting_.rotate.max.x);
        ImGui::DragFloatRange2("spped", &setting_.spped.min, &setting_.spped.max);
        ImGui::DragFloat3("direction_min", &setting_.direction.min.x);
        ImGui::DragFloat3("direction_max", &setting_.direction.max.x);
        ImGui::DragFloat3("acceleration_min", &setting_.acceleration.min.x);
        ImGui::DragFloat3("acceleration_max", &setting_.acceleration.max.x);
        ImGui::ColorEdit4("color_min", &setting_.color.min.x);
        ImGui::ColorEdit4("color_max", &setting_.color.max.x);

        if (ImGui::Button("save"))
        {
            ConfigManager::GetInstance()->SaveData(name_);
        }

        if (ImGui::Button("add"))
        {
            std::vector<Particle> particles;

            for (uint32_t count = 0; count < countPerEmit_; ++count)
            {
                particles.push_back(GenerateParticleData());
            }

            ParticleManager::GetInstance()->AddParticleToGroup(name_, particles);
            currentTime_ = 0;
        }
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    ImGui::End();
}

void ParticleEmitter::Draw()
{
#ifndef _DEBUG
    return;
#endif // _DEBUG

    switch (shape_)
    {
    case EmitterShape::Box:
        {
            Matrix4x4 affine = MakeAffineMatrix(size_, rotate_, position_);
            LineDrawer::GetInstance()->DrawOBB(affine);
        }
        break;
    case EmitterShape::Shpere:
        Matrix4x4 affine = MakeAffineMatrix({ radius_ }, rotate_, position_ );
        LineDrawer::GetInstance()->DrawSphere(affine);
        break;
    case EmitterShape::Circle:
        break;
    case EmitterShape::None:
        break;
    default:
        break;
    }
}

void ParticleEmitter::SetShape_Box(const Vector3& _size)
{
    shape_ = EmitterShape::Box;
    size_ = _size;
}

void ParticleEmitter::SetShape_Sphere(float _radius)
{
    shape_ = EmitterShape::Shpere;
    radius_ = _radius;
}

void ParticleEmitter::SetShape_Circle(float _radius)
{
    shape_ = EmitterShape::Circle;
    radius_ = _radius;
}

Particle ParticleEmitter::GenerateParticleData()
{
    Particle particle;

    auto random = RandomGenerator::GetInstance();

    float lifeTIme          = random->GetUniformFloat   (setting_.lifeTime.min      , setting_.lifeTime.max     );
    Vector3 size            = random->GetUniformVec3    (setting_.size.min          , setting_.size.max         );
    Vector3 rotate          = random->GetUniformVec3    (setting_.rotate.min        , setting_.rotate.max       );
    float speed             = random->GetUniformFloat   (setting_.spped.min         , setting_.spped.max        );
    Vector3 direction       = random->GetUniformVec3    (setting_.direction.min     , setting_.direction.max    );
    Vector3 acceleration    = random->GetUniformVec3    (setting_.acceleration.min  , setting_.acceleration.max );


    Vector4 color;
    if (randomColor_)
        color = random->GetUniformColor();
    else
        color = random->GetUniformVec4(setting_.color.min, setting_.color.max);


    Vector3 pos{};
    switch (shape_)
    {
    case EmitterShape::Box:
        {
            Vector3 halfSize = size_ / 2.0f;
            pos = random->GetUniformVec3(-halfSize, halfSize);
        }
        break;
    case EmitterShape::Shpere:
        {
            float rad = random->GetUniformAngle();
            pos.x = std::cosf(rad);
            pos.y = random->GetUniformFloat(-radius_, radius_);
            pos.z = std::sinf(rad);
        }
        break;
    case EmitterShape::Circle:
        {
            float rad = random->GetUniformAngle();
            pos.x = std::cosf(rad);
            pos.y = 0;
            pos.z = std::sinf(rad);
        }
        break;
    case EmitterShape::None:
    default:
        throw std::runtime_error("Unknown Emitter Shape");
        break;
    }

    pos += position_;

    particle.Initialize(lifeTIme, size, rotate, pos, color, speed, direction, acceleration);
    return particle;
}