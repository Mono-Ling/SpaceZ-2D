#include "Core/Object/PhyObject/Transform.h"
using namespace Core::Math;
using namespace std;

namespace Core
{
    Matrix3x3 Transform::LocalToWorldMatrix() const
    {
        UpdateMatrix();
        return _local2world;
    }
    Matrix3x3 Transform::WorldToLocalMatrix() const
    {
        UpdateMatrix();
        return _world2local;
    }
    Vector2 Transform::Position() const
    {
        if(auto parent = _parent.lock())
            return MulPoint(parent->LocalToWorldMatrix(), _position);
        return  _position;
    }
    float Transform::Rotation() const
    {
        if(auto parent = _parent.lock())
            return parent->LocalToWorldMatrix().GetRotate() + _rotation;
        return _rotation;
    }
    Vector2 Transform::Scale() const
    {
        if(auto parent = _parent.lock())
        {
            auto parentScale = parent->LocalToWorldMatrix().GetScale();
            return Vector2(parentScale.x * _scale.x, parentScale.y * _scale.y);
        }
        return _scale;
    }
    void Transform::LocalPosition(const Vector2& pos)
    {
        if(_position == pos)
            return;
        _position = pos;
        _dirty = true;
    }
    void Transform::LocalRotation(float rot)
    {
        if(Near(_rotation, rot))
            return;
        _rotation = rot;
        _dirty = true;
    }
    void Transform::LocalScale(const Vector2& scale)
    {
        if(_scale == scale)
            return;
        _scale = scale;
        _dirty = true;
    }
    void Transform::SetParent(const shared_ptr<Transform>& parent)
    {
        _parent.reset();
        _parent = parent;
    }

    void Transform::UpdateMatrix() const
    {
        if(_dirty)
        {
            _local2parent = TRS(_position, _rotation, _scale);
            _parent2local = InverseTRS(_position, _rotation, _scale);

            _dirty = false;
        }

        auto parent = _parent.lock();
        if(!parent)
        {
            _local2world = _local2parent;
            _world2local = _parent2local;
            return;
        }
        _local2world = parent->LocalToWorldMatrix() * _local2parent;
        _world2local = _parent2local * parent->WorldToLocalMatrix();
    }
}