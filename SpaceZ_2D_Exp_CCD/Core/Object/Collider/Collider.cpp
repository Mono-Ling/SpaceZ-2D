#include "Object/Collider.h"
#include "Math/Matrix3x3.h"
using namespace Core::Math;
using namespace Core::Collision;

namespace Core
{
    Bound Collider::SweepBound(const Vector2& dx, float da) const
    {
        auto local2world = transform.LocalToWorldMatrix();
        auto world2local = transform.WorldToLocalMatrix();
        auto start = BakeBound(local2world, world2local);

        local2world = GetTranslateMatrix(dx) * local2world * GetRotateMatrix(da);
        world2local = GetRotateMatrix(-da) * world2local * GetTranslateMatrix(-dx);
        auto end = BakeBound(local2world, world2local);

        return start + end;
    }
    ConvexDetectReq Collider::CreateConvexDetectReq() const
    {
        return {ConvexPtr(), transform.LocalToWorldMatrix(), transform.WorldToLocalMatrix()};
    }

    Bound Collider::BakeBound(const Matrix3x3& local2world, const Matrix3x3& world2local) const
    {
        Vector2 max
        {
            Support(Vector2::right, local2world, world2local).x,
            Support(Vector2::up, local2world, world2local).y,
        };
        Vector2 min
        {
            Support(Vector2::left, local2world, world2local).x,
            Support(Vector2::down, local2world, world2local).y,
        };
        Bound bound;
        bound.SetBound(min, max);
        return bound;
    }
}