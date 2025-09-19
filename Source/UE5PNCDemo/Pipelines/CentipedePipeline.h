// Define a few PNC components we will use with our chunks.
#pragma once
#include "CoreMinimal.h"
#include "UE5PNC/public/PNCDefault.h"

#include "Algorithms/CentipedeLogic.h"
#include "Algorithms/ComputeSoftBodyInstancedMatirx.h"
#include "Algorithms/ConstrainSoftBodyPositions.h"
#include "Algorithms/InitCentipede.h"
#include "Algorithms/AddDragToVelocity.h"
#include "Algorithms/SoftBodyRelax.h"
#include "Algorithms/ApplyVelocityOnPosition.h"
#include "Algorithms/ApplyGravityOnVelocity.h"
#include "Algorithms/PropagateTransform.h"
#include "Algorithms/AnimateCentipedeLegs.h"

struct CentipedePipeline : public PNC::Pipeline<CentipedePipeline>
{
public:
    UCentipedesPNC* UComponent;
    float DeltaTime;

    PNC::AlgorithmCacheRouter<CentipedeLogic> CentipedeLogicRouter;
    PNC::AlgorithmCacheRouter<AddDragToVelocity> AddDragToVelocityRouter;
    PNC::AlgorithmCacheRouter<ApplyGravityOnVelocity> ApplyGravityOnVelocityRouter;
    PNC::AlgorithmCacheRouter<RelaxSoftBody> RelaxSoftBodyRouter;
    PNC::AlgorithmCacheRouter<ApplyVelocityOnPosition> ApplyVelocityOnPositionRouter;
    PNC::AlgorithmCacheRouter<ComputeSoftBodyInstancedMatirx> ComputeSoftBodyInstancedMatirxRouter;
    PNC::AlgorithmCacheRouter<PropagateTransformFromParentChunk> PropagateTransformFromParentChunkRouter;
    PNC::AlgorithmCacheRouter<AnimateCentipedeLegs> AnimateCentipedeLegsRouter;
    
    template<typename T>
    bool Requirements(T req)
    {
        // If any algorithm matches, this pipeline is matching.
        bool bMatch = false;
        bMatch |= req.Algorithm(CentipedeLogicRouter);
        bMatch |= req.Algorithm(AddDragToVelocityRouter);
        bMatch |= req.Algorithm(ApplyGravityOnVelocityRouter);
        bMatch |= req.Algorithm(RelaxSoftBodyRouter);
        bMatch |= req.Algorithm(ApplyVelocityOnPositionRouter);
        bMatch |= req.Algorithm(ComputeSoftBodyInstancedMatirxRouter);
        return bMatch;
    }

    CentipedePipeline(UCentipedesPNC* uComponent)
        : UComponent(uComponent)
    {
    }

    void Execute(PNC::KChunkTree& chunk)
    {
        CentipedeLogicRouter.Run(CentipedeLogic(UComponent, DeltaTime), chunk);
        CopyPreviousPosition().Run(chunk);
        int32 iLoop = 0;
        auto dt = DeltaTime;
        while (dt > 0)
        {
            auto stepDeltaTime = FMath::Min(dt, UComponent->MaxDeltaTimeStep);
            dt -= stepDeltaTime;
            ApplyGravityOnVelocityRouter.Run(ApplyGravityOnVelocity(stepDeltaTime * UComponent->Gravity), chunk);
            AddDragToVelocityRouter.Run(AddDragToVelocity(UComponent, stepDeltaTime), chunk);
            RelaxSoftBodyRouter.Run(RelaxSoftBody(UComponent, stepDeltaTime), chunk);
            ApplyVelocityOnPositionRouter.Run(ApplyVelocityOnPosition(stepDeltaTime), chunk);
            ++iLoop;
        }
        ComputeSoftBodyInstancedMatirxRouter.Run(chunk);
        auto firstChild = chunk.GetFirstChildChunk();
        auto currentChild = firstChild;
        if (currentChild != nullptr)
            do
            {
                AnimateCentipedeLegsRouter.TryRun(AnimateCentipedeLegs(UComponent), *currentChild);
                PropagateTransformFromParentChunkRouter.TryRun(PropagateTransformFromParentChunk(), *currentChild);
                currentChild = currentChild->GetNextSiblingChunk();
            } while (currentChild != firstChild);
    }
};
