#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Math/Vector2.h"

namespace UnityPlugin
{
    struct BodyState
    {
        Core::RigidbodyHandle handle;
        Core::Math::Vector2 position;
        float rotation = 0.0f;
        Core::Math::Vector2 linearVelocity;
        float angularVelocity = 0.0f;
    };
}