#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"



struct AnimateCentipedeLegs : public Ni::Algorithm<AnimateCentipedeLegs>
{
    UCentipedesPNC* UComponent;

    Ni::CoSingleParentOutsideChunk* SingleParentOutsideChunk;

    CoLocalTransform* LocalTransform;

    CoCentipedeLegNode* CentipedeLegNode;

    CoCentipedeBodyNode* ParentCentipedeBodyNode;
    AnimateCentipedeLegs(UCentipedesPNC* uComponent)
        :UComponent(uComponent)
    {
    }

    template<typename T>
    bool Requirements(T req)
    {
        if (!req.Component(SingleParentOutsideChunk)) return false;
        if (!req.Component(LocalTransform)) return false;
        if (!req.Component(CentipedeLegNode)) return false;
        if (!req.ParentComponent(ParentCentipedeBodyNode)) return false;
        return true;
    }
    void Execute(int count)const
    {
        Execute(count, false, 0);

    }
    void Execute(int count, bool useSegmentData, Size_t segmentIndexData)const
    {
        ALGO_PROFILE(TEXT("AnimateCentipedeLegs"));
        for (int32 i = 0; i < count; ++i)
        {
            CoCentipedeLegNode& legNode = CentipedeLegNode[i];

            auto segmentIndex = SingleParentOutsideChunk->Index;

            auto dist = ParentCentipedeBodyNode[segmentIndex].Displacement;

            legNode.Phase += dist * UComponent->LegDistanceFrequency;
            
            legNode.Phase = FMath::Modulo(legNode.Phase, 1.0f);
            if (legNode.Phase < 0)
                legNode.Phase = 1 + legNode.Phase;

            auto phaseWithSegment = legNode.Phase + (useSegmentData ? segmentIndexData : segmentIndex) * UComponent->LegSegmentFrequency;
            auto phase = FMath::Modulo(phaseWithSegment, 1.0f);
            if (phase < 0)
                phase = 1 + legNode.Phase;


            int32 phaseStep = (int32)(phase * 4);
            float phaseLocal = (phase - phaseStep * 0.25f) / 0.25f;
            phaseStep %= 4;

            switch (legNode.JointIndex)
            {
                case 0:
                {
                    float tY = 0;
                    float tZ = 0;
                    switch (phaseStep) 
                    {
                    case 0:
                        tY = 0;
                        tZ = (1 - phaseLocal) * 0.5f;
                        break;
                    case 1:
                        tY = phaseLocal;
                        tZ = phaseLocal * 0.5f;
                        break;
                    case 2:
                        tY = 1 - phaseLocal;
                        tZ = phaseLocal * 0.5f + 0.5f;
                        break;
                    case 3:
                        tY = 0;
                        tZ = (1 - phaseLocal) * 0.5f + 0.5f;
                        break;
                    }

                    auto node0ZAngle = FMath::Lerp(UComponent->LegNode0ZRotationMinMax.X, UComponent->LegNode0ZRotationMinMax.Y, legNode.Chirality ? tZ : 1-tZ);
                    auto node0YAngle = FMath::Lerp(UComponent->LegNode0YRotationMinMax.X, UComponent->LegNode0YRotationMinMax.Y, tY);
                    auto angles = legNode.RotationEulerBase + FVector(0, node0YAngle, node0ZAngle);
                    auto rot = FQuat::MakeFromEuler(angles);
                    rot.DiagnosticCheckNaN();
                    LocalTransform[i].Value.SetRotation(rot);
                    break;
                }
                case 1:
                {
                    float tY = 0;
                    switch (phaseStep)
                    {
                    case 0:
                        tY = 0;
                        break;
                    case 1:
                        tY = phaseLocal;
                        break;
                    case 2:
                        tY = 1 - phaseLocal;
                        break;
                    case 3:
                        tY = 0;
                        break;
                    }

                    auto node1YAngle = FMath::Lerp(UComponent->LegNode1YRotationMinMax.X, UComponent->LegNode1YRotationMinMax.Y, tY);
                    auto angles = legNode.RotationEulerBase + FVector(0, node1YAngle, 0);
                    auto rot = FQuat::MakeFromEuler(angles);
                    rot.DiagnosticCheckNaN();
                    LocalTransform[i].Value.SetRotation(rot);
                    break;
                }
            }
        }
    }
};