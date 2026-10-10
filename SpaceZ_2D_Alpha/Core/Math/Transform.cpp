#include "Math/Transform.h"
#include "Math/Matrix3x3.h"
#include "Math/Matrix2x2.h"
#include "Math/Math.h"

namespace Core::Math
{
    Vector2 Transform::WorldPosition() const
    {
        return MulPoint(_parentToWorld, _position);
    }
    float Transform::WorldRotation() const
    {
        return _parentToWorld.GetRotate() + _rotation;
    }
    Vector2 Transform::WorldScale() const
    {
        auto parentS = _parentToWorld.GetScale();
        return Vector2(parentS.x * _scale.x, parentS.y * _scale.y);
    }

    void Transform::SetLocalPosition(const Vector2& pos)
    {
        _position = pos;
        _dirty = true;
    }
    void Transform::SetLocalRotation(float rot)
    {
        _rotation = rot;
        _dirty = true;
    }
    void Transform::SetLocalScale(const Vector2& scale)
    {
        _scale = Vector2(Max(Epsilon, scale.x), Max(Epsilon, scale.y));
        _dirty = true;
    }
    void Transform::SetParentTransform(const Transform& parent)
    {
        _parentToWorld = parent.GetLocalToWorldMatrix();
        _worldToParent = parent.GetWorldToLocalMatrix();
        _dirty = true;
    }
    Matrix3x3 Transform::GetLocalToWorldMatrix() const
    {
        UpdateMatrix();
        return _localToWorld;
    }
    Matrix3x3 Transform::GetWorldToLocalMatrix() const
    {
        UpdateMatrix();
        return _worldToLocal;
    }
    Vector2 Transform::PointLocalToWorld(const Vector2& point) const
    {
        UpdateMatrix();
        return MulPoint(_localToWorld, point);
    }
    Vector2 Transform::DirLocalToWorld(const Vector2& dir) const
    {
        UpdateMatrix();
        return MulVector(_localToWorld, dir);
    }

    Vector2 Transform::PointWorldToLocal(const Vector2& point) const
    {
        UpdateMatrix();
        return MulPoint(_worldToLocal, point);
    }
    Vector2 Transform::DirWorldToLocal(const Vector2& dir) const
    {
        UpdateMatrix();
        return MulVector(_worldToLocal, dir);
    }
    Vector2 Transform::NormalWorldToLocal(const Vector2& normal) const
    {
        UpdateMatrix();
        auto m = _localToWorld.ToMatrix2x2();
        m.TransposeSelf();
        return m * normal;
    }
    Vector2 Transform::PointWorldToLocalRT(const Vector2& point) const
    {
        UpdateMatrix();
        auto m = InverseTR(_localToWorld.GetPosition(), _localToWorld.GetRotate());
        return MulPoint(m, point);
    }

    void Transform::UpdateMatrix() const
    {
        if(!_dirty)
            return;
        _localToParent = TRS(_position, _rotation, _scale);
        _parentToLocal = InverseTRS(_position, _rotation, _scale);

        _localToWorld = _parentToWorld * _localToParent;
        _worldToLocal = _parentToLocal * _worldToParent;

        _dirty = false;
    }
}