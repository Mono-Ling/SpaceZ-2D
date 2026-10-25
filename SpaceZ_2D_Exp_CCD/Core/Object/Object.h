#pragma once
#include "Object/Handle.h"

namespace Core
{
    class Rigidbody;
    class Collider;

    using RigidbodyHandle = Handle<Rigidbody>;
    using ColliderHandle = Handle<Collider>;
}