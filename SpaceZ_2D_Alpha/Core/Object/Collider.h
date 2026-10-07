#pragma once
#include "Object/Object.h"

namespace Core
{
    class Collider
    {
    public:
        float staticFriction;
        float dynamicFriction;
        float elasticity;

    private:
        ColliderHandle _handle;
        
    };
}