#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"



struct AnimateCentipedeLegs : public PNC::Algorithm<AnimateCentipedeLegs>
{
    UCentipedesPNC* UComponent;

    PNC::CoSingleParentOutsideChunk* SingleParentOutsideChunk;
    PNC::CoParentInChunk* ParentInChunk;

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
        if (!req.Component(ParentInChunk)) return false;
        if (!req.Component(LocalTransform)) return false;
        if (!req.Component(CentipedeLegNode)) return false;
        if (!req.ParentComponent(ParentCentipedeBodyNode)) return false;
        return true;
    }

    void Execute(int count)const
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AnimateCentipedeLegs"));
        for (int32 i = 0; i < count; ++i)
        {
            CoCentipedeLegNode& legNode = CentipedeLegNode[i];

            auto segmentIndex = SingleParentOutsideChunk->Index;

            auto dist = ParentCentipedeBodyNode[segmentIndex].Displacement;

            legNode.Phase += dist * UComponent->LegDistanceFrequency;
            legNode.Phase = FMath::Modulo(legNode.Phase, 1.0f);

            auto phaseWithSegment = legNode.Phase + segmentIndex * UComponent->LegSegmentFrequency;
            auto phase = FMath::Modulo(phaseWithSegment, 1.0f);


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

            //if (phase < 0.25f)
            //{
            //    auto phaseLocal = phase / 0.25f;
            //}
            //else
            //{
            //    auto phaseOut = (phase - 0.5f) / 0.5f;
            //    //move from back to from
            //    switch (legNode.JointIndex)
            //    {
            //    case 0:
            //        auto node0ZAngle = FMath::Lerp(UComponent->LegNode0ZRotationMinMax.Y, UComponent->LegNode0ZRotationMinMax.X, phaseOut);
            //        auto node0YAngle = FMath::Lerp(UComponent->LegNode0YRotationMinMax.Y, UComponent->LegNode0YRotationMinMax.X, phaseOut);
            //        auto angles = legNode.RotationEulerBase + FVector(0, node0YAngle, node0ZAngle);
            //        LocalTransform[i].Value.SetRotation(FQuat::MakeFromEuler(angles));
            //        break;
            //    }
            //}
            //if (phase < 0.5f)
            //{
            //    auto phaseIn = phase / 0.5f;
            //    //move from back to from
            //    switch (legNode.JointIndex)
            //    {
            //    case 0:
            //        auto node0ZAngle = FMath::Lerp(UComponent->LegNode0ZRotationMinMax.X, UComponent->LegNode0ZRotationMinMax.Y, phaseIn);
            //        auto node0YAngle = FMath::Lerp(UComponent->LegNode0YRotationMinMax.X, UComponent->LegNode0YRotationMinMax.Y, phaseIn);
            //        auto angles = legNode.RotationEulerBase + FVector(0, node0YAngle, node0ZAngle);
            //        LocalTransform[i].Value.SetRotation(FQuat::MakeFromEuler(angles));
            //        break;
            //    }
            //}
            //else
            //{
            //    auto phaseOut = (phase - 0.5f) / 0.5f;
            //    //move from back to from
            //    switch (legNode.JointIndex)
            //    {
            //    case 0:
            //        auto node0ZAngle = FMath::Lerp(UComponent->LegNode0ZRotationMinMax.Y, UComponent->LegNode0ZRotationMinMax.X, phaseOut);
            //        auto node0YAngle = FMath::Lerp(UComponent->LegNode0YRotationMinMax.Y, UComponent->LegNode0YRotationMinMax.X, phaseOut);
            //        auto angles = legNode.RotationEulerBase + FVector(0, node0YAngle, node0ZAngle);
            //        LocalTransform[i].Value.SetRotation(FQuat::MakeFromEuler(angles));
            //        break;
            //    }
            //}
            //ParentVelocity[leg.]
        }
    }
};