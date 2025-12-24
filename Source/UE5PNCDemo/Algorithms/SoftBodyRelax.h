#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will alter the velocity of each SoftBodyNode so they respect their length constraint.
/// </summary>
struct RelaxSoftBody : public Ni::Algorithm<RelaxSoftBody>
{
    UCentipedesPNC* UComponent;
    float DeltaTime;

    CoVelocity* Velocity;
    CoPosition* Position;
    CoSoftBodyNode* SoftBodyNode;

    RelaxSoftBody(UCentipedesPNC* aUComponent, float aDeltaTime)
        : UComponent(aUComponent)
        , DeltaTime(aDeltaTime)
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Position)) return false;
        if (!req.Component(SoftBodyNode)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        ALGO_PROFILE(TEXT("RelaxSoftBody"));
        // First node is the head and should not be moved by the relax algorithm
        auto previousPosition = Position[0].Position;
        for (int i = 1; i < count; ++i)
        {
            auto currentPosition = Position[i].Position + Velocity[i].Velocity * DeltaTime;
            auto towardPrevious = previousPosition - currentPosition;
            float l = towardPrevious.Length();
            float offset = l - SoftBodyNode[i].Length;
            if (l < 0.001f)
                towardPrevious = FVector(-1, 0, 0);
            else
                towardPrevious.Normalize();

            //auto fixedPosition = currentPosition + offset * towardPrevious;
            //Velocity[i].Velocity = (fixedPosition - Position[i].Position) / DeltaTime;
            auto force = offset * towardPrevious;
            Velocity[i].Velocity += force / DeltaTime;
            previousPosition = Position[i].Position + Velocity[i].Velocity * DeltaTime;
        }
    }
};