#pragma once

#include "Vector3.h"
#include "Quaternion.h"

#include <vector>
#include <map>

struct aiAnimation;

class ModelAnimation
{
public:

    ModelAnimation() = default;
    ~ModelAnimation() = default;

    void Initialize();
    void Update(const std::string& _rootNodeName);
    void Draw();

    void ReadAnimation(const aiAnimation* _animation);

    Matrix4x4 GetLocalMatrix() const { return localMatrix_; }

private:
    template <typename T>
    struct Keyframe
    {
        float time;
        T value;
    };
    using KeyframeVector3 = Keyframe<Vector3>;
    using KeyframeQuaternion = Keyframe<Quaternion>;

    template <typename T>
    struct AnimationCurve
    {
        std::vector<Keyframe<T>> keyframes;
    };
    struct NodeAnimation
    {
        AnimationCurve<Vector3> translate;
        AnimationCurve<Quaternion> rotation;
        AnimationCurve<Vector3> scale;
    };
    struct Animation
    {
        float duration; //全体の尺
        std::map<std::string, NodeAnimation> nodeAnimations;
    };

    Animation animation_;
    Matrix4x4 localMatrix_;
    float animetionTImer_ = 0.0f;

    Vector3 CalculateValue(const AnimationCurve<Vector3>& _curve, float _time);
    Quaternion CalculateValue(const AnimationCurve<Quaternion>& _curve, float _time);

};