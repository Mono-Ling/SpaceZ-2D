#pragma once
#include "Object/Rigidbody.h"
#include "Math/Vector2.h"

namespace Export
{
    struct ExportBodyInfo
    {
        Core::RigidbodyHandle handle;
        float rotation = 0.0f;
        float angularVelocity = 0.0f;
        Core::Math::Vector2 position;
        Core::Math::Vector2 linearVelocity;

        inline static ExportBodyInfo From(
            const Core::RigidbodyHandle& handle,
            const Core::Math::Vector2& pos, float rot,
            const Core::Math::Vector2& lv, float av)
        {
            ExportBodyInfo info;
            info.handle = handle;
            info.position = pos;
            info.rotation = rot;
            info.linearVelocity = lv;
            info.angularVelocity = av;
            return info;
        }
    };
}