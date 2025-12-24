#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will apply a gravity vector to the velocity of the chunk.
/// The gravity vector given to this algorithm must be multiplied by the delta time
/// </summary>
struct ApplyGravityOnVelocity : public Ni::Algorithm<ApplyGravityOnVelocity>
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
        ALGO_PROFILE(TEXT("ApplyGravityOnVelocity"));
        for (int i = 0; i < count; ++i)
        {
            Velocity[i].Velocity += DeltaGravity * Mass[i].Mass;
        }
    }
};