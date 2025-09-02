#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Control the behaviour of centipede.
/// It will move to random target infinitely
/// </summary>
struct CentipedeLogic : public PNC::ChunkAlgorithm<CentipedeLogic>
{
    UCentipedesPNC* UComponent;
    float DeltaTime;

    CoCentipede* Centipede;
    CoVelocity* Velocity;
    CoPosition* Position;

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Centipede)) return false;
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Position)) return false;
        return true;
    }

    CentipedeLogic(UCentipedesPNC* aUComponent, float aDeltaTime)
        : UComponent(aUComponent)
        , DeltaTime(aDeltaTime)
    {
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("CentipedeLogic"));
        Centipede->Target.DiagnosticCheckNaN();
        Centipede->Timer -= DeltaTime;
        if (Centipede->Timer < 0)
            PickNewTarget();

        FVector rootPosition = Centipede->HeadPosition;
        auto diff = Centipede->Target - rootPosition;
        auto dist = diff.Length();
        auto moveDistance = Centipede->Speed * UComponent->CentipedeLogicSpeed * DeltaTime;
        if (dist < 0.01f || moveDistance >= dist)
        {
            // reached target;
            PickNewTarget();
        }
        else
        {
            //move
            auto dir = diff / dist;
            rootPosition += moveDistance * dir;
            rootPosition.DiagnosticCheckNaN();
        }
        Centipede->HeadPosition = rootPosition;
        // Move the first node to the new root position.
        Position[0].Position = rootPosition;
        Velocity[0].Velocity = FVector::Zero();
    }

    void PickNewTarget()const
    {
        Centipede->Speed = FMath::RandRange(UComponent->RandomSpeedRange.X, UComponent->RandomSpeedRange.X + UComponent->RandomSpeedRange.Y);

        auto pos = Centipede->HeadPosition;
        auto bX = FMath::RandBool();
        auto bY = FMath::RandBool();
        auto bZ = FMath::RandBool();
        auto min = -UComponent->RandomTargetRange;
        min.X = bX ? min.X : pos.X;
        min.Y = bY ? min.Y : pos.Y;
        min.Z = bZ ? min.Z : pos.Z;
        auto max = UComponent->RandomTargetRange;
        max.X = bX ? pos.X : max.X;
        max.Y = bY ? pos.Y : max.Y;
        max.Z = bZ ? pos.Z : max.Z;

        Centipede->Target = FMath::RandPointInBox(FBox(min, max));
        Centipede->Timer = FMath::RandRange(UComponent->RandomRetargetTime.X, UComponent->RandomRetargetTime.X + UComponent->RandomRetargetTime.Y);
    }
};