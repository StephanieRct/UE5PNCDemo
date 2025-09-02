#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will apply a gravity vector to the velocity of the chunk.
/// The gravity vector given to this algorithm must be multiplied by the delta time
/// </summary>
struct ApplyGravityOnVelocity : public PNC::ChunkAlgorithm<ApplyGravityOnVelocity>
{
    FVector DeltaGravity;

    CoVelocity* Velocity;
    CoMass* Mass;

    ApplyGravityOnVelocity(FVector aDeltaGravity)
        : DeltaGravity(aDeltaGravity) 
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Mass)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ApplyGravityOnVelocity"));
        for (int i = 0; i < count; ++i)
        {
            Velocity[i].Velocity += DeltaGravity * Mass[i].Mass;
        }
    }
};