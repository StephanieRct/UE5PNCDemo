#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will apply the velocity on the position with a given delta time.
/// </summary>
struct ApplyVelocityOnPosition : public Ni::Algorithm<ApplyVelocityOnPosition>
{
    float DeltaTime;

    CoVelocity* Velocity;
    CoPosition* Position;

    ApplyVelocityOnPosition(float aDeltaTime)
        : DeltaTime(aDeltaTime) 
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Position)) return false;
        if (!req.Component(Velocity)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        ALGO_PROFILE(TEXT("ApplyVelocityOnPosition"));
        for (int i = 0; i < count; ++i)
        {
            Position[i].Position += Velocity[i].Velocity * DeltaTime;
        }
    }
};