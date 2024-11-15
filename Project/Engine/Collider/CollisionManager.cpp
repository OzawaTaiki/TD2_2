#include "CollisionManager.h"
#include "Collider.h"
#include "VectorFunction.h"

CollisionManager* CollisionManager::GetInstance()
{
    static CollisionManager instance;
    return &instance;
}

uint32_t CollisionManager::GetAtttibute(const std::string& _atrribute)
{
    auto it = atrributeMap_.find(_atrribute);
    if (it != atrributeMap_.end())
        return it->second;

    atrributeMap_[_atrribute] = static_cast <uint32_t>(1 << atrributeMap_.size());
    uint32_t result = atrributeMap_[_atrribute];
    return result;
}

uint32_t CollisionManager::GetMask(const std::string& _atrribute)
{
    return ~GetAtttibute(_atrribute);
}

void CollisionManager::CheckAllCollision()
{
    for (auto itA = colliders_.begin(); itA != colliders_.end(); itA++)
    {
        for (auto itB = std::next(itA); itB != colliders_.end(); itB++)
        {
            if ((*itA)->GetMask() & (*itB)->GetAtrribute_() ||
                (*itB)->GetMask() & (*itA)->GetAtrribute_())
            {
                CheckCollisionPair(*itA, *itB);
            }
        }
    }
}

void CollisionManager::CheckCollisionPair(Collider* _colliderA, Collider* _colliderB)
{
    switch (_colliderA->GetBoundingBox())
    {
    case Collider::BoundingBox::Sphere_3D:
        {
            Sphere sphereA = _colliderA->GetShape<Sphere>();
            sphereA.center = Transform({ 0,0,0 }, _colliderA->GetWorldMatrix());

            switch (_colliderB->GetBoundingBox())
            {
            case Collider::BoundingBox::Sphere_3D:
                {
                    Sphere sphereB = _colliderB->GetShape<Sphere>();
                    sphereB.center = Transform({ 0,0,0 }, _colliderB->GetWorldMatrix());

                    if (IsCollision(sphereA, sphereB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                }
                break;
            case Collider::BoundingBox::AABB_3D:
                {
                    AABB aabbB = _colliderB->GetShape<AABB>();
                    aabbB.center = Transform({ 0,0,0 }, _colliderB->GetWorldMatrix());

                    if (IsCollision(sphereA, aabbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::OBB_3D:
                {
                    OBB obbB = _colliderB->GetShape<OBB>();
                    //obbB.center = Transform(obbB.referencePoint, _colliderB->GetWorldMatrix());
                    if (IsCollision(sphereA, obbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::NONE:
                assert(false && "Please set BoundingBox before SetShape");
                break;
            default:
                break;
                }
            }
        break;
    case Collider::BoundingBox::AABB_3D:
        {
            AABB aabbA = _colliderA->GetShape<AABB>();
            switch (_colliderB->GetBoundingBox())
            {
            case Collider::BoundingBox::Sphere_3D:
                {
                    Sphere sphereB = _colliderB->GetShape<Sphere>();
                    if (IsCollition(aabbA, sphereB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::AABB_3D:
                {
                    AABB aabbB = _colliderB->GetShape<AABB>();
                    if (IsCollision(aabbA, aabbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::OBB_3D:
                {
                    OBB obbB = _colliderB->GetShape<OBB>();
                    if (IsCollision(aabbA, obbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::NONE:
                assert(false && "Please set BoundingBox before SetShape");
                break;
            default:
                break;
            }
        }
        break;
    case Collider::BoundingBox::OBB_3D:
            {  OBB obbA = _colliderA->GetShape<OBB>();
            switch (_colliderB->GetBoundingBox())
            {
            case Collider::BoundingBox::Sphere_3D:
                {
                    Sphere sphereB = _colliderB->GetShape<Sphere>();
                    if (IsCollision(obbA, sphereB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::AABB_3D:
                {
                    AABB aabbB = _colliderB->GetShape<AABB>();
                    if (IsCollision(obbA, aabbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;
                }
            case Collider::BoundingBox::OBB_3D:
                {
                    OBB obbB = _colliderB->GetShape<OBB>();
                    if (IsCollision(obbA, obbB))
                    {
                        _colliderA->OnCollision();
                        _colliderB->OnCollision();
                    }
                    break;


                }
                break;
            }
        }
        break;
    case Collider::BoundingBox::NONE:
        assert(false && "Please set BoundingBox before SetShape");
        break;
    default:
        break;
    }
};

bool CollisionManager::IsCollision(const Sphere& _sphere1, const Sphere& _sphere2)
{
    return false;
}

bool CollisionManager::IsCollision(const Sphere& _sphere, const AABB& _aabb)
{
    return false;
}

bool CollisionManager::IsCollision(const Sphere& _sphere, const OBB& _obb)
{
    return false;
}

bool CollisionManager::IsCollition(const AABB& _aabb, const Sphere& _sphere)
{
    return false;
}

bool CollisionManager::IsCollision(const AABB& _aabb1, const AABB& _aabb2)
{
    return false;
}

bool CollisionManager::IsCollision(const AABB& _aabb, const OBB& _obb)
{
    return false;
}

bool CollisionManager::IsCollision(const OBB& _obb, const Sphere& _sphere)
{
    return false;
}

bool CollisionManager::IsCollision(const OBB& _obb, const AABB& _aabb)
{
    return false;
}

bool CollisionManager::IsCollision(const OBB& _obb1, const OBB& _obb2)
{
    return false;
}

