#pragma once
#include "DrawDebugHelpers.h"
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Control the behaviour of centipede.
/// It will move to random target infinitely
/// </summary>
struct CentipedeLogic : public Ni::Algorithm<CentipedeLogic>
{
    UCentipedesPNC* UComponent;
    float DeltaTime;

    CoCentipede* Centipede;
    CoCentipedeBodyNode* CentipedeBodyNode;
    CoVelocity* Velocity;
    CoPosition* Position;
    CoPositionPrevious* PositionPrevious;

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Centipede)) return false;
        if (!req.Component(CentipedeBodyNode)) return false;
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Position)) return false;
        if (!req.Component(PositionPrevious)) return false;
        return true;
    }

    CentipedeLogic(UCentipedesPNC* aUComponent, float aDeltaTime)
        : UComponent(aUComponent)
        , DeltaTime(aDeltaTime)
    {
    }

    void Execute(const Size_t count)const 
    {
        ALGO_PROFILE(TEXT("CentipedeLogic"));
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
            CentipedeBodyNode[0].Displacement = 0;
        }
        else
        {
            //move
            auto dir = diff / dist;
            rootPosition += moveDistance * dir;
            CentipedeBodyNode[0].Displacement = moveDistance;
        }
        Centipede->HeadPosition = rootPosition;
        // Move the first node to the new root position.
        Position[0].Position = rootPosition;
        Velocity[0].Velocity = FVector::Zero();

        // Compute each body node displacement
        auto positionA = PositionPrevious[0].Position;
        for (int i = 1; i < count; ++i)
        {
            auto positionB = PositionPrevious[i].Position;
            auto towardA = positionA - positionB;
            auto l = towardA.Length();
            if (l < 0.001f)
                CentipedeBodyNode[i].Displacement = 0;
            else
            {
                towardA = towardA / l;
                auto disp = Position[i].Position - positionB;
                CentipedeBodyNode[i].Displacement = FVector::DotProduct(disp, towardA);
                //const auto& trf = UComponent->GetOwner()->GetTransform();
                //DrawDebugLine(
                //    UComponent->GetWorld(),
                //    trf.TransformPosition(Position[i].Position),
                //    trf.TransformPosition(Position[i].Position + towardA * CentipedeBodyNode[i].Displacement),
                //    CentipedeBodyNode[i].Displacement >= 0 ? FColor::Blue : FColor::Red,
                //    false, // Not persistent (drawn per frame)
                //    0.f,   // Only for this frame
                //    255,
                //    5.f    // Thickness of 5
                //);
            }
            positionA = positionB;
        }
    }

    void PickNewTarget()const
    {

        auto forward = Position[0].Position - Position[1].Position;
        if (forward.SquaredLength() < 0.001)
            forward = FVector(1, 0, 0);
        else
            forward.Normalize();

        int32 maxTry = 16;
        float d = 0;
        do
        {
            Centipede->Speed = FMath::RandRange(UComponent->RandomSpeedRange.X, UComponent->RandomSpeedRange.X + UComponent->RandomSpeedRange.Y);

            auto pos = Centipede->HeadPosition;
            auto bX = FMath::RandBool();
            auto bY = FMath::RandBool();
            auto bZ = FMath::RandBool();
            auto min = -UComponent->RandomTargetRange;
            min.Z = 0;
            min.X = bX ? min.X : pos.X;
            min.Y = bY ? min.Y : pos.Y;
            min.Z = bZ ? min.Z : pos.Z;
            auto max = UComponent->RandomTargetRange;
            max.Z += UComponent->RandomTargetRange.Z;
            max.X = bX ? pos.X : max.X;
            max.Y = bY ? pos.Y : max.Y;
            max.Z = bZ ? pos.Z : max.Z;
            Centipede->Target = FMath::RandPointInBox(FBox(min, max));
            auto newForward = Centipede->Target - Position[0].Position;
            if (newForward.SquaredLength() < 0.001)
                newForward = FVector(1, 0, 0);
            else
                newForward.Normalize();
            d = FVector::DotProduct(forward, newForward);
        } while (d < -0.5f && maxTry-- > 0);

        Centipede->Timer = FMath::RandRange(UComponent->RandomRetargetTime.X, UComponent->RandomRetargetTime.X + UComponent->RandomRetargetTime.Y);
    }
};