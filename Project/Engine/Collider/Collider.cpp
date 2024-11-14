#include "Collider.h"
#include "CollisionManager.h"

void Collider::Draw()
{
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Sphere>)
        {
            Sphere sphere = arg;
            //ImGui::Text("Sphere");
            //ImGui::Text("Center: %f, %f, %f", sphere.center.x, sphere.center.y, sphere.center.z);
            //ImGui::Text("Radius: %f", sphere.radius);
        }
        else if constexpr (std::is_same_v<T, AABB>)
        {
            AABB aabb = arg;
            //ImGui::Text("AABB");
            //ImGui::Text("Min: %f, %f, %f", aabb.min.x, aabb.min.y, aabb.min.z);
            //ImGui::Text("Max: %f, %f, %f", aabb.max.x, aabb.max.y, aabb.max.z);
        }
        else if constexpr (std::is_same_v<T, OBB>)
        {
            OBB obb = arg;
            //ImGui::Text("OBB");
            //ImGui::Text("Center: %f, %f, %f", obb.center.x, obb.center.y, obb.center.z);
            //ImGui::Text("Size: %f, %f, %f", obb.size.x, obb.size.y, obb.size.z);
        }
        else
        {
            assert(false && "Draw() is not supported in this type");
        }
               }, shape_);
}

void Collider::SetShape(float _radius)
{
    switch (boundingBox_)
    {
    case Collider::BoundingBox::Sphere_3D:
        shape_ = Sphere{ _radius };
        break;
    case Collider::BoundingBox::AABB_3D:
    case Collider::BoundingBox::OBB_3D:
        assert(false && "SetShape(float _radius) is not supported in AABB_3D and OBB_3D");
        break;
    case Collider::BoundingBox::NONE:
        assert(false && "Please set BoundingBox before SetShape");
        break;
    default:
        break;
    }
}

void Collider::SetShape(const Vector3& _min, const Vector3& _max)
{
    switch (boundingBox_)
    {
    case Collider::BoundingBox::Sphere_3D:
        assert(false && "SetShape(const Vector3& _min, const Vector3& _max) is not supported in Sphere_3D");
        break;
    case Collider::BoundingBox::AABB_3D:
        shape_ = AABB(_min, _max);
        break;
    case Collider::BoundingBox::OBB_3D:
        shape_ = OBB(_min, _max);
        break;
    case Collider::BoundingBox::NONE:
        assert(false && "Please set BoundingBox before SetShape");
        break;
    default:
        break;
    }
}


void Collider::SetAtrribute(const std::string& _atrribute)
{
    atrribute_ = CollisionManager::GetInstance()->GetAtttibute(_atrribute);
}

void Collider::SetMask(const std::string& _atrribute)
{
    mask_ = CollisionManager::GetInstance()->GetMask(_atrribute);
}

void Collider::SetMask(std::initializer_list<std::string> _atrribute)
{
    mask_ = 0xffffffffu;
    for (std::string str : _atrribute)
    {
        mask_ &= CollisionManager::GetInstance()->GetMask(str);
    }
}

void Collider::SetReferencePoint(const Vector3& _referencePoint)
{
    if (boundingBox_ == BoundingBox::OBB_3D ||
        boundingBox_ == BoundingBox::AABB_3D)
    {
        std::visit([_referencePoint](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, OBB>)
            {
                OBB obb = arg;
                obb.referencePoint[0] = _referencePoint.x;
                obb.referencePoint[1] = _referencePoint.y;
                obb.referencePoint[2] = _referencePoint.z;
            }
            else if constexpr (std::is_same_v<T, AABB>)
            {
                AABB aabb = arg;
                aabb.referencePoint[0] = _referencePoint.x;
                aabb.referencePoint[1] = _referencePoint.y;
                aabb.referencePoint[2] = _referencePoint.z;
            }
            else if constexpr (std::is_same_v<T, Sphere>)
            {
                assert(false && "SetReferencePoint is supported in OBB_3D or AABB_3D");
            }
                   }, shape_);
    }
    else
    {
        assert(false && "SetReferencePoint is supported in OBB_3D or AABB_3D");
    }
}
