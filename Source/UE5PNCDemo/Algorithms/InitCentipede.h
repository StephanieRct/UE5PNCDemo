#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Initialize a centipede chunk
/// </summary>
struct InitCentipede : public PNC::ChunkAlgorithm<InitCentipede>
{
    UCentipedesPNC* UComponent;

    CoCentipede* Centipede;
    CoPosition* Position;
    CoVelocity* Velocity;
    CoMass* Mass;
    CoSoftBodyNode* SoftBodyNode;
    FInstancedStaticMeshInstanceData* InstancedStaticMeshInstanceData;

    InitCentipede(UCentipedesPNC* uComponent)
        : UComponent(uComponent) 
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Centipede)) return false;
        if (!req.Component(Position)) return false;
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Mass)) return false;
        if (!req.Component(SoftBodyNode)) return false;
        if (!req.Component(InstancedStaticMeshInstanceData)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("CreateCentipede"));
        Centipede->HeadPosition = FMath::RandPointInBox(FBox(-UComponent->RandomTargetRange, UComponent->RandomTargetRange));
        Centipede->Speed = 1;
        Centipede->Target = Centipede->HeadPosition;
        float mass = FMath::RandRange(UComponent->CentipedeMassRange.X, UComponent->CentipedeMassRange.X + UComponent->CentipedeMassRange.Y);
        FVector previousPosition = Centipede->HeadPosition;
        for (int i = 0; i < count; ++i)
        {
            float massMul = FMath::RandRange(UComponent->CentipedeSegmentMassVariation.X, UComponent->CentipedeSegmentMassVariation.X + UComponent->CentipedeSegmentMassVariation.Y);
            Mass[i].Mass = mass * massMul;
            InstancedStaticMeshInstanceData[i] = FInstancedStaticMeshInstanceData(FMatrix::Identity);
            SoftBodyNode[i].Length = UComponent->CentipedeSegmentLength;
            SoftBodyNode[i].Up = FVector(0, 0, 1);
            Position[i].Position = previousPosition + FVector(-SoftBodyNode[i].Length, 0, 0);
            Velocity[i].Velocity = FVector(0, 0, 0);
            previousPosition = Position[i].Position;
        }
    }
};