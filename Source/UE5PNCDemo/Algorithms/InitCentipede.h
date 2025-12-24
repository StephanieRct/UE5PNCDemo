#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Initialize a centipede chunk
/// </summary>
struct InitCentipede : public Ni::Algorithm<InitCentipede>
{
    UCentipedesPNC* UComponent;

    CoCentipede* Centipede;
    CoCentipedeBodyNode* CentipedeBodyNode;
    CoPosition* Position;
    CoVelocity* Velocity;
    CoMass* Mass;
    CoSoftBodyNode* SoftBodyNode;
    CoFInstancedStaticMeshInstanceData* InstancedStaticMeshInstanceData;


    InitCentipede(UCentipedesPNC* uComponent)
        : UComponent(uComponent) 
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Centipede)) return false;
        if (!req.Component(CentipedeBodyNode)) return false;
        if (!req.Component(Position)) return false;
        if (!req.Component(Velocity)) return false;
        if (!req.Component(Mass)) return false;
        if (!req.Component(SoftBodyNode)) return false;
        if (!req.Component(InstancedStaticMeshInstanceData)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        ALGO_PROFILE(TEXT("CreateCentipede"));
        Centipede->HeadPosition = FMath::RandPointInBox(FBox(-UComponent->RandomTargetRange, UComponent->RandomTargetRange));
        Centipede->HeadPosition.Z += UComponent->RandomTargetRange.Z;
        Centipede->Speed = 1;
        Centipede->Target = Centipede->HeadPosition;
        float mass = FMath::RandRange(UComponent->CentipedeMassRange.X, UComponent->CentipedeMassRange.X + UComponent->CentipedeMassRange.Y);
        FVector previousPosition = Centipede->HeadPosition;
        for (int i = 0; i < count; ++i)
        {
            CentipedeBodyNode[i].Displacement = 0;
            float massMul = FMath::RandRange(UComponent->CentipedeSegmentMassVariation.X, UComponent->CentipedeSegmentMassVariation.X + UComponent->CentipedeSegmentMassVariation.Y);
            Mass[i].Mass = mass * massMul;
            InstancedStaticMeshInstanceData[i].Transform = FMatrix::Identity;
            SoftBodyNode[i].Length = UComponent->CentipedeSegmentLength;
            SoftBodyNode[i].Up = FVector(0, 0, 1);
            Position[i].Position = previousPosition + FVector(-SoftBodyNode[i].Length, 0, 0);
            Velocity[i].Velocity = FVector(0, 0, 0);
            previousPosition = Position[i].Position;
        }

    }
};


struct InitCentipedeLegs : public Ni::Algorithm<InitCentipedeLegs>
{
    int32 SegmentPerCentipede;
    int32 LegPerSegment;
    int32 NodePerLeg;
    float NodeLength;
    FVector LegScale;
    FVector LegScale2;

    Ni::CoSingleParentOutsideChunk* SingleParentOutsideChunk;
    Ni::CoParentInChunk* ParentInChunk;
    CoCentipedeLegNode* CentipedeLegNode;
    CoLocalTransform* LocalTransform;
    CoFInstancedStaticMeshInstanceData* InstancedStaticMeshInstanceData;
    Ni::Size_t ChunkIndex;

    InitCentipedeLegs(int32 segmentPerCentipede, int32 legPerSegment, int32 nodePerLeg, float nodeLength, FVector legScale, FVector legScale2)
        : SegmentPerCentipede(segmentPerCentipede)
        , LegPerSegment(legPerSegment)
        , NodePerLeg(nodePerLeg)
        , NodeLength(nodeLength)
        , LegScale(legScale)
        , LegScale2(legScale2)
    {
    }

    template<typename T>
    bool Requirements(T req)
    {
        if (!req.Component(SingleParentOutsideChunk)) return false;
        if (!req.Component(ParentInChunk)) return false;
        if (!req.Component(LocalTransform)) return false;
        if (!req.Component(InstancedStaticMeshInstanceData)) return false;
        if (!req.Component(CentipedeLegNode)) return false;
        if (!req.ChunkIndex(ChunkIndex)) return false;
        return true;
    }

    void Execute(int count)const
    {
        ALGO_PROFILE(TEXT("InitCentipedeLegs"));
        
        // this chunk's parent node is the chunk index
        SingleParentOutsideChunk->Index = ChunkIndex;

        float step = 200.0f / LegPerSegment;
        int32 iRootL = 0;
        int32 iRootR = NodePerLeg;
        int32 iRootStep = NodePerLeg * 2;
        {
            FVector positionLegRootL = FVector(-100 + 0.5f * step, -100, -75);
            FVector positionLegRootR = FVector(-100 + 0.5f * step, 100, -75);
            FVector positionLegRootStep = FVector(step, 0, 0);
            for (int32 iLeg = 0; iLeg < LegPerSegment; ++iLeg)
            {
                CentipedeLegNode[iRootL].JointIndex = 0;
                CentipedeLegNode[iRootR].JointIndex = 0;
                CentipedeLegNode[iRootL].Chirality = 0;
                CentipedeLegNode[iRootR].Chirality = 1;
                CentipedeLegNode[iRootL].RotationEulerBase = FVector(0, 0, -90);
                CentipedeLegNode[iRootR].RotationEulerBase = FVector(0, 0, 90);

                LocalTransform[iRootL].Value = FTransform(FQuat::MakeFromEuler(CentipedeLegNode[iRootL].RotationEulerBase), positionLegRootL, LegScale);
                LocalTransform[iRootR].Value = FTransform(FQuat::MakeFromEuler(CentipedeLegNode[iRootR].RotationEulerBase), positionLegRootR, LegScale);
                ParentInChunk[iRootL].Index = -1;
                ParentInChunk[iRootR].Index = -1;
                
                for (int32 iNode = 1; iNode < NodePerLeg; ++iNode)
                {
                    auto iL = iRootL + iNode;
                    auto iR = iRootR + iNode;
                    CentipedeLegNode[iL].JointIndex = iNode;
                    CentipedeLegNode[iR].JointIndex = iNode;
                    CentipedeLegNode[iL].Chirality = 0;
                    CentipedeLegNode[iR].Chirality = 1;
                    CentipedeLegNode[iL].RotationEulerBase = FVector(0, 0, 0);
                    CentipedeLegNode[iR].RotationEulerBase = FVector(0, 0, 0);
                    LocalTransform[iL].Value = FTransform(FQuat::Identity, FVector(NodeLength, 0, 0), LegScale2);
                    LocalTransform[iR].Value = FTransform(FQuat::Identity, FVector(NodeLength, 0, 0), LegScale2);
                    ParentInChunk[iL].Index = iL - 1;
                    ParentInChunk[iR].Index = iR - 1;
                }

                positionLegRootL += positionLegRootStep;
                positionLegRootR += positionLegRootStep;
                iRootL += iRootStep;
                iRootR += iRootStep;
            }
        }

        for (int32 i = 0; i < count; ++i)
        {
            InstancedStaticMeshInstanceData[i].Transform = FMatrix::Identity;
            CentipedeLegNode[i].Phase = 0;
        }

    }
};

