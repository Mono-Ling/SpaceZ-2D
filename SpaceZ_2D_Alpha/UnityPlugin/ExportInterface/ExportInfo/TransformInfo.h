#pragma once
#include "Math/Vector2.h"
namespace Export
{
    struct TransformInfo
    {
        float rotation;
        Core::Math::Vector2 position;
        Core::Math::Vector2 scale;
    };
}