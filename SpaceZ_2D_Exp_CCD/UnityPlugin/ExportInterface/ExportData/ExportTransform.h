#pragma once
#include "Math/Vector2.h"

namespace ExportData
{
    struct ExportTransform
    {
        Core::Math::Vector2 position;
        float rotation = 0.0f;
        Core::Math::Vector2 scale;
    };
}