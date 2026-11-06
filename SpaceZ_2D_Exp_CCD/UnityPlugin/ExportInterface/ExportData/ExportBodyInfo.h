#pragma once
#include "Math/Vector2.h"

namespace ExportData
{
    struct ExportBodyInfo
    {
        Core::Math::Vector2 position;
        float rotation = 0.0f;
        
        Core::Math::Vector2 linearVelocity;
        float angularVelocity = 0.0f;
    };
}