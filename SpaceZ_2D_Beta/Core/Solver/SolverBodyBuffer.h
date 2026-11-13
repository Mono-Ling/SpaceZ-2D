#pragma once
#include "Core/Solver/SolverRigidbody.h"
#include "Core/Object/Container/ObjectBuffer.h"
#include "Core/Object/Container/PoolPtr.h"

namespace Core::Solver
{
    class SolverBodyBuffer
    : public Container::ObjectBuffer<SolverRigidbody, Container::PoolPtr<SolverRigidbody>>
    {
    public:
        SolverBodyBuffer()
        : ObjectBuffer<SolverRigidbody, Container::PoolPtr<SolverRigidbody>>(
            [](const Container::PoolPtr<SolverRigidbody>& ptr)->SolverRigidbody*
            { return ptr.GetPtr(); }) {}
    };
}