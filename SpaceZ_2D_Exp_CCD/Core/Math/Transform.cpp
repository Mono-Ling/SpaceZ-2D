#include "Math/Transform.h"
#include "Math/Math.h"

namespace Core::Math
{
    Matrix3x3 Transform::LocalToWorldMatrix() const
    {
        if(_dirty)
            UpdateMatrix();
        return _localToWorld;
    }
    Matrix3x3 Transform::WorldToLocalMatrix() const
    {
        if(_dirty)
            UpdateMatrix();
        return _worldToLocal;
    }
    Vector2 Transform::Position() const
    {
        return MulPoint(_parentToWorld, _position);
    }
    float Transform::Rotation() const
    {
        return _parentToWorld.GetRotate() + _rotation;
    }
    Vector2 Transform::Scale() const
    {
        if(_dirty)
            UpdateMatrix();
        return _localToWorld.GetScale();
    }
    void Transform::Position(const Vector2& pos)
    {
        _position = MulPoint(_worldToParent, pos);
        _dirty = true;
    }
    void Transform::Rotation(float rot)
    {
        _rotation = rot - _parentToWorld.GetRotate();
        _dirty = true;
    }
    void Transform::LocalPosition(const Vector2& pos)
    {
        _position = pos;
        _dirty = true;
    }
    void Transform::LocalRotation(float rot)
    {
        _rotation = rot;
        _dirty = true;
    }
     void Transform::LocalScale(const Vector2& scale)
     {
        _scale = Vector2(Abs(scale.x), Abs(scale.y));
        _dirty = true;
     }
    void Transform::SetParentTransform(const Transform& parent)
    {
        _parentToWorld = parent.LocalToWorldMatrix();
        _worldToParent = parent.WorldToLocalMatrix();
        UpdateMatrix();
    }

    void Transform::UpdateMatrix() const
    {
        auto localToParent = TRS(_position, _rotation, _scale);
        auto parentToLocal = InverseTRS(_position, _rotation, _scale);

        _localToWorld = _parentToWorld * localToParent;
        _worldToLocal = parentToLocal * _worldToParent;
        _dirty = false;
    }
}