#include "CentipedesPNC.h"
#include "components/components.h"
#include "Pipelines/CentipedePipeline.h"
#include "UE5PNC/public/PncAPI.h"
#include "MyGameSubsystem.h"
#include "Async/ParallelFor.h"
#include "CentipedeConfig.h"
#include "Misc/Parse.h"
#include "Misc/CommandLine.h"

void FCentipede::Init(UCentipedesPNC* uComponent, int segmentCount)
{
    uComponent->AddObjectCallbacks(this);
    Centipede.HeadPosition = FMath::RandPointInBox(FBox(-uComponent->RandomTargetRange, uComponent->RandomTargetRange));
    Centipede.HeadPosition.Z += uComponent->RandomTargetRange.Z;
    Centipede.Speed = 1;
    Centipede.Target = Centipede.HeadPosition;
    float mass = FMath::RandRange(uComponent->CentipedeMassRange.X, uComponent->CentipedeMassRange.X + uComponent->CentipedeMassRange.Y);
    FVector previousPosition = Centipede.HeadPosition;

    FBodyNode* lastBodyNode = nullptr;
    for (int i = 0; i < segmentCount; ++i)
    {
        uComponent->DoRandomAlloc();
        TUniquePtr<FBodyNode> bodyNode = MakeUnique<FBodyNode>();
        FBodyNode* n = bodyNode.Get();
        uComponent->AddObjectCallbacks(n);
        if (lastBodyNode == nullptr)
            RootBodyNode = MoveTemp(bodyNode);
        else
        {
            lastBodyNode->NextBodyNode = MoveTemp(bodyNode);
            n->PreviousBodyNode = lastBodyNode;
        }

        n->CentipedeBodyNode.Displacement = 0;
        float massMul = FMath::RandRange(uComponent->CentipedeSegmentMassVariation.X, uComponent->CentipedeSegmentMassVariation.X + uComponent->CentipedeSegmentMassVariation.Y);
        n->Centipede = this;
        n->Mass.Mass = mass * massMul;
        n->SoftBodyNode.Length = uComponent->CentipedeSegmentLength;
        n->SoftBodyNode.Up = FVector(0, 0, 1);
        n->Position.Position = previousPosition + FVector(-n->SoftBodyNode.Length, 0, 0);
        n->Velocity.Velocity = FVector(0, 0, 0);
        n->Rotation.Rotation = FQuat::Identity;
        n->Scale.Scale = FVector::One();
        previousPosition = n->Position.Position;

        float step = 200.0f / uComponent->LegPerSegment;
        {
            FVector positionLegRootL = FVector(-100 + 0.5f * step, -100, -75);
            FVector positionLegRootR = FVector(-100 + 0.5f * step, 100, -75);
            FVector positionLegRootStep = FVector(step, 0, 0);
            for (uint32 iLeg = 0; iLeg < uComponent->LegPerSegment; ++iLeg)
            {
                uComponent->DoRandomAlloc();
                uint32 indexL = n->RootLegNodes.Add(MakeUnique<FLegNode>());
                FLegNode* rootLegL = n->RootLegNodes[indexL].Get();
                uComponent->AddObjectCallbacks(rootLegL);
                rootLegL->BodyNode = n;
                rootLegL->CentipedeLegNode.JointIndex = 0;
                rootLegL->CentipedeLegNode.Chirality = 0;
                rootLegL->CentipedeLegNode.Phase = 0;
                rootLegL->CentipedeLegNode.RotationEulerBase = FVector(0, 0, -90);
                rootLegL->LocalTransform.Value = FTransform(FQuat::MakeFromEuler(rootLegL->CentipedeLegNode.RotationEulerBase), positionLegRootL, uComponent->LegScale);


                uComponent->DoRandomAlloc();
                uint32 indexR = n->RootLegNodes.Add(MakeUnique<FLegNode>());
                FLegNode* rootLegR = n->RootLegNodes[indexR].Get();
                uComponent->AddObjectCallbacks(rootLegR);
                rootLegR->BodyNode = n;
                rootLegR->CentipedeLegNode.JointIndex = 0;
                rootLegR->CentipedeLegNode.Chirality = 1;
                rootLegR->CentipedeLegNode.Phase = 0;
                rootLegR->CentipedeLegNode.RotationEulerBase = FVector(0, 0, 90);
                rootLegR->LocalTransform.Value = FTransform(FQuat::MakeFromEuler(rootLegR->CentipedeLegNode.RotationEulerBase), positionLegRootR, uComponent->LegScale);

                for (uint32 iNode = 1; iNode < uComponent->NodePerLeg; ++iNode)
                {
                    uComponent->DoRandomAlloc();
                    TUniquePtr<FLegNode> legL = MakeUnique<FLegNode>();
                    uComponent->AddObjectCallbacks(legL.Get());
                    legL->BodyNode = n;
                    legL->CentipedeLegNode.JointIndex = iNode;
                    legL->CentipedeLegNode.Chirality = 0;
                    legL->CentipedeLegNode.Phase = 0;
                    legL->CentipedeLegNode.RotationEulerBase = FVector(0, 0, 0);
                    legL->LocalTransform.Value = FTransform(FQuat::Identity, FVector(uComponent->LegNodeLength, 0, 0), uComponent->LegScale2);
                    rootLegL->ChildNode = MoveTemp(legL);
                    rootLegL = rootLegL->ChildNode.Get();

                    uComponent->DoRandomAlloc();
                    TUniquePtr<FLegNode> legR = MakeUnique<FLegNode>();
                    uComponent->AddObjectCallbacks(legR.Get());
                    legR->BodyNode = n;
                    legR->CentipedeLegNode.JointIndex = iNode;
                    legR->CentipedeLegNode.Chirality = 1;
                    legR->CentipedeLegNode.Phase = 0;
                    legR->CentipedeLegNode.RotationEulerBase = FVector(0, 0, 0);
                    legR->LocalTransform.Value = FTransform(FQuat::Identity, FVector(uComponent->LegNodeLength, 0, 0), uComponent->LegScale2);
                    rootLegR->ChildNode = MoveTemp(legR);
                    rootLegR = rootLegR->ChildNode.Get();
                }

                positionLegRootL += positionLegRootStep;
                positionLegRootR += positionLegRootStep;
            }
        }
        lastBodyNode = n;
    }
}

void FCentipede::BeginFrame(UCentipedesPNC* uComponent, float deltaTime)
{
    CentipedeLogic centipedeLogic(uComponent, deltaTime);
    centipedeLogic.Centipede = &Centipede;
    centipedeLogic.CentipedeBodyNode = &RootBodyNode->CentipedeBodyNode;
    centipedeLogic.Velocity = &RootBodyNode->Velocity;
    centipedeLogic.Position = &RootBodyNode->Position;
    centipedeLogic.PositionPrevious = &RootBodyNode->PositionPrevious;
    centipedeLogic.Execute(1);

    ////
    // Compute each body node displacement
    FBodyNode* n = RootBodyNode.Get();
    auto positionA = n->Position.Position;
    while (n != nullptr)
    {
        auto positionB = n->PositionPrevious.Position;
        auto towardA = positionA - positionB;
        auto l = towardA.Length();
        if (l < 0.001f)
            n->CentipedeBodyNode.Displacement = 0;
        else
        {
            towardA = towardA / l;
            auto disp = n->Position.Position - positionB;
            n->CentipedeBodyNode.Displacement = FVector::DotProduct(disp, towardA);
        }
        positionA = positionB;
        n = n->NextBodyNode.Get();
    }

    n = RootBodyNode.Get();
    while (n != nullptr)
    {
        CopyPreviousPosition copyPreviousPosition;
        copyPreviousPosition.Position = &n->Position;
        copyPreviousPosition.PositionPrevious = &n->PositionPrevious;
        copyPreviousPosition.Execute(1);
        n = n->NextBodyNode.Get();
    }
    /////

}
//
//void FBodyNode::UpdatePhysics(UCentipedesPNC* uComponent, float deltaTime)
//{
//
//}
void FCentipede::UpdatePhysics(UCentipedesPNC* uComponent, float deltaTime)
{
    FBodyNode* n = nullptr;
    int32 iLoop = 0;
    auto dt = deltaTime;
    //while (dt > 0)
    {
        auto stepDeltaTime = FMath::Min(dt, uComponent->MaxDeltaTimeStep);
        dt -= stepDeltaTime;

        // ApplyGravityOnVelocity
        // AddDragToVelocity
        n = RootBodyNode.Get();
        while (n != nullptr)
        {

            ApplyGravityOnVelocity aApplyGravityOnVelocity(stepDeltaTime * uComponent->Gravity);
            aApplyGravityOnVelocity.Velocity = &n->Velocity;
            aApplyGravityOnVelocity.Mass = &n->Mass;
            aApplyGravityOnVelocity.Execute(1);

            AddDragToVelocity aAddDragToVelocity(uComponent, stepDeltaTime);
            aAddDragToVelocity.Velocity = &n->Velocity;
            aAddDragToVelocity.Execute(1);

            n = n->NextBodyNode.Get();
        }

        /////
        // Relax
        n = RootBodyNode.Get();
        auto previousPosition = n->Position.Position;
        while (n != nullptr)
        {
            auto currentPosition = n->Position.Position + n->Velocity.Velocity * stepDeltaTime;
            auto towardPrevious = previousPosition - currentPosition;
            float l = towardPrevious.Length();
            float offset = l - n->SoftBodyNode.Length;
            if (l < 0.001f)
                towardPrevious = FVector(-1, 0, 0);
            else
                towardPrevious.Normalize();

            auto force = offset * towardPrevious;
            n->Velocity.Velocity += force / stepDeltaTime;
            previousPosition = n->Position.Position + n->Velocity.Velocity * stepDeltaTime;
            n = n->NextBodyNode.Get();
        }
        /////
        // ApplyVelocityOnPosition
        n = RootBodyNode.Get();
        while (n != nullptr)
        {
            ApplyVelocityOnPosition aApplyVelocityOnPosition(stepDeltaTime);
            aApplyVelocityOnPosition.Velocity = &n->Velocity;
            aApplyVelocityOnPosition.Position = &n->Position;
            aApplyVelocityOnPosition.Execute(1);
            n = n->NextBodyNode.Get();
        }
        ++iLoop;
    }
    /////
    // ComputeSoftBodyInstancedMatirx
    n = RootBodyNode.Get();
    auto pos0 = n->Position.Position;
    auto pos1 = n->NextBodyNode->Position.Position;
    FVector previousPosition;
    bool bFirst = true;
    while (n != nullptr)
    {
        auto currentPosition = n->Position.Position;
        auto forward = bFirst ? pos0 - pos1
            : previousPosition - currentPosition;
        bFirst = false;
        float l = forward.Length();
        if (l < 0.001f)
            forward = FVector(-1, 0, 0);
        else
            forward /= l;
        auto up = FVector(0, 0, 1);
        if (FVector::DotProduct(forward, up) >= 0.998f)
            up = n->SoftBodyNode.Up;
        auto right = FVector::CrossProduct(up, forward);
        up = FVector::CrossProduct(forward, right);
        up.Normalize();
        right.Normalize();
        n->SoftBodyNode.Up = up;
        auto matrix = FMatrix(forward, right, up, currentPosition);
        FTransform trf = FTransform(matrix);
        n->Rotation.Rotation = trf.GetRotation();
        n->Scale.Scale = trf.GetScale3D();
        previousPosition = currentPosition;
        n = n->NextBodyNode.Get();
    }
}

void FLegNode::UpdateAnimation(UCentipedesPNC* uComponent, float deltaTime)
{
    AnimateCentipedeLegs aAnimateCentipedeLegs(uComponent);
    Ni::CoSingleParentOutsideChunk cCoSingleParentOutsideChunk;
    cCoSingleParentOutsideChunk.Index = 0;
    Ni::Size_t segmentIndex = 0;
    aAnimateCentipedeLegs.SingleParentOutsideChunk = &cCoSingleParentOutsideChunk;
    aAnimateCentipedeLegs.ParentCentipedeBodyNode = &BodyNode->CentipedeBodyNode;
    aAnimateCentipedeLegs.LocalTransform = &LocalTransform;
    aAnimateCentipedeLegs.CentipedeLegNode = &CentipedeLegNode;
    aAnimateCentipedeLegs.Execute(1, true, segmentIndex);
}

//void FCentipede::UpdateAnimation(UCentipedesPNC* uComponent, float deltaTime)
//{
//    Ni::Size_t segmentIndex = 0;
//    FBodyNode* n = RootBodyNode.Get();
//    while (n != nullptr)
//    {
//        for (const TUniquePtr<FLegNode>& ll : n->RootLegNodes)
//        {
//            FLegNode* l = ll.Get();
//            while (l != nullptr)
//            {
//                l->UpdateAnimation(uComponent, deltaTime);
//                l = l->ChildNode.Get();
//            }
//        }
//
//        //    //ComputeSoftBodyInstancedMatirxRouter.Run(chunk);
//        //    //auto firstChild = chunk.GetFirstChildChunk();
//        //    //auto currentChild = firstChild;
//        //    //if (currentChild != nullptr)
//        //    //    do
//        //    //    {
//        //    //        AnimateCentipedeLegsRouter.TryRun(AnimateCentipedeLegs(UComponent), *currentChild);
//        //    //        PropagateTransformFromParentChunkRouter.TryRun(PropagateTransformFromParentChunk(), *currentChild);
//        //    //        currentChild = currentChild->GetNextSiblingChunk();
//        //    //    } while (currentChild != firstChild);
//        n = n->NextBodyNode.Get();
//        ++segmentIndex;
//    }
//}
//
void FCentipede::Update(UCentipedesPNC* uComponent, float deltaTime)
{
    CentipedeLogic centipedeLogic(uComponent, deltaTime);
    centipedeLogic.Centipede = &Centipede;
    centipedeLogic.CentipedeBodyNode= &RootBodyNode->CentipedeBodyNode;
    centipedeLogic.Velocity = &RootBodyNode->Velocity;
    centipedeLogic.Position = &RootBodyNode->Position;
    centipedeLogic.PositionPrevious = &RootBodyNode->PositionPrevious;
    centipedeLogic.Execute(1);

    ////
    // Compute each body node displacement
    FBodyNode* n = RootBodyNode.Get();
    auto positionA = n->Position.Position;
    while (n != nullptr)
    {
        auto positionB = n->PositionPrevious.Position;
        auto towardA = positionA - positionB;
        auto l = towardA.Length();
        if (l < 0.001f)
            n->CentipedeBodyNode.Displacement = 0;
        else
        {
            towardA = towardA / l;
            auto disp = n->Position.Position - positionB;
            n->CentipedeBodyNode.Displacement = FVector::DotProduct(disp, towardA);
        }
        positionA = positionB;
        n = n->NextBodyNode.Get();
    }

    n = RootBodyNode.Get();
    while (n != nullptr)
    {
        CopyPreviousPosition copyPreviousPosition;
        copyPreviousPosition.Position = &n->Position;
        copyPreviousPosition.PositionPrevious = &n->PositionPrevious;
        copyPreviousPosition.Execute(1);
        n = n->NextBodyNode.Get();
    }
    /////

        
    int32 iLoop = 0;
    auto dt = deltaTime;
    //while (dt > 0)
    {
        auto stepDeltaTime = FMath::Min(dt, uComponent->MaxDeltaTimeStep);
        dt -= stepDeltaTime;

        // ApplyGravityOnVelocity
        // AddDragToVelocity
        n = RootBodyNode.Get();
        while (n != nullptr)
        {

            ApplyGravityOnVelocity aApplyGravityOnVelocity(stepDeltaTime * uComponent->Gravity);
            aApplyGravityOnVelocity.Velocity = &n->Velocity;
            aApplyGravityOnVelocity.Mass = &n->Mass;
            aApplyGravityOnVelocity.Execute(1);

            AddDragToVelocity aAddDragToVelocity(uComponent, stepDeltaTime);
            aAddDragToVelocity.Velocity = &n->Velocity;
            aAddDragToVelocity.Execute(1);

            n = n->NextBodyNode.Get();
        }

        /////
        // Relax
        n = RootBodyNode.Get();
        auto previousPosition = n->Position.Position;
        while (n != nullptr)
        {
            auto currentPosition = n->Position.Position + n->Velocity.Velocity * stepDeltaTime;
            auto towardPrevious = previousPosition - currentPosition;
            float l = towardPrevious.Length();
            float offset = l - n->SoftBodyNode.Length;
            if (l < 0.001f)
                towardPrevious = FVector(-1, 0, 0);
            else
                towardPrevious.Normalize();

            auto force = offset * towardPrevious;
            n->Velocity.Velocity += force / stepDeltaTime;
            previousPosition = n->Position.Position + n->Velocity.Velocity * stepDeltaTime;
            n = n->NextBodyNode.Get();
        }
        /////
        // ApplyVelocityOnPosition
        n = RootBodyNode.Get();
        while (n != nullptr)
        {
            ApplyVelocityOnPosition aApplyVelocityOnPosition(stepDeltaTime);
            aApplyVelocityOnPosition.Velocity = &n->Velocity;
            aApplyVelocityOnPosition.Position = &n->Position;
            aApplyVelocityOnPosition.Execute(1);
            n = n->NextBodyNode.Get();
        }
        ++iLoop;
    }

    /////
    // ComputeSoftBodyInstancedMatirx
    n = RootBodyNode.Get();
    auto pos0 = n->Position.Position;
    auto pos1 = n->NextBodyNode->Position.Position;
    FVector previousPosition;
    bool bFirst = true;
    while (n != nullptr)
    {
        auto currentPosition = n->Position.Position;
        auto forward = bFirst ? pos0 - pos1
            : previousPosition - currentPosition;
        bFirst = false;
        float l = forward.Length();
        if (l < 0.001f)
            forward = FVector(-1, 0, 0);
        else
            forward /= l;
        auto up = FVector(0, 0, 1);
        if (FVector::DotProduct(forward, up) >= 0.998f)
            up = n->SoftBodyNode.Up;
        auto right = FVector::CrossProduct(up, forward);
        up = FVector::CrossProduct(forward, right);
        up.Normalize();
        right.Normalize();
        n->SoftBodyNode.Up = up;
        auto matrix = FMatrix(forward, right, up, currentPosition);
        FTransform trf = FTransform(matrix);
        n->Rotation.Rotation = trf.GetRotation();
        n->Scale.Scale = trf.GetScale3D();
        previousPosition = currentPosition;
        n = n->NextBodyNode.Get();
    }

    n = RootBodyNode.Get();
    AnimateCentipedeLegs aAnimateCentipedeLegs(uComponent);
    Ni::CoSingleParentOutsideChunk cCoSingleParentOutsideChunk;
    cCoSingleParentOutsideChunk.Index = 0;
    Ni::Size_t segmentIndex = 0;
    while (n != nullptr)
    {
        for (const TUniquePtr<FLegNode>& ll : n->RootLegNodes)
        {
            FLegNode* l = ll.Get();
            while (l != nullptr)
            {
                aAnimateCentipedeLegs.SingleParentOutsideChunk = &cCoSingleParentOutsideChunk;
                aAnimateCentipedeLegs.ParentCentipedeBodyNode = &n->CentipedeBodyNode;
                aAnimateCentipedeLegs.LocalTransform = &l->LocalTransform;
                aAnimateCentipedeLegs.CentipedeLegNode = &l->CentipedeLegNode;
                aAnimateCentipedeLegs.Execute(1, true, segmentIndex);
                l = l->ChildNode.Get();
            }
        }
        
        //    //ComputeSoftBodyInstancedMatirxRouter.Run(chunk);
        //    //auto firstChild = chunk.GetFirstChildChunk();
        //    //auto currentChild = firstChild;
        //    //if (currentChild != nullptr)
        //    //    do
        //    //    {
        //    //        AnimateCentipedeLegsRouter.TryRun(AnimateCentipedeLegs(UComponent), *currentChild);
        //    //        PropagateTransformFromParentChunkRouter.TryRun(PropagateTransformFromParentChunk(), *currentChild);
        //    //        currentChild = currentChild->GetNextSiblingChunk();
        //    //    } while (currentChild != firstChild);
        n = n->NextBodyNode.Get();
        ++segmentIndex;
    }
}

UCentipedesPNC::UCentipedesPNC()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCentipedesPNC::BeginPlay()
{
    Super::BeginPlay();
    UPncAPI* pnc = UPncAPI::Get();
    if (!pnc)
        return;
    if (bUseCommandLine)
    {
        FParse::Value(FCommandLine::Get(), TEXT("pnc-chunk-count="), CentipedeChunkCount);
        FParse::Value(FCommandLine::Get(), TEXT("pnc-cent-per-chunk="), CentipedeCountPerChunk);
        FParse::Value(FCommandLine::Get(), TEXT("pnc-seg-per-cent="), CentipedeSegmentsMin);
        FParse::Value(FCommandLine::Get(), TEXT("pnc-seg-length="), CentipedeSegmentLength);
        int iUseOOP;
        if (FParse::Value(FCommandLine::Get(), TEXT("pnc-oop="), iUseOOP))
        {
            bUseOOP = iUseOOP != 0;
        }
        int iMultithread;
        if (FParse::Value(FCommandLine::Get(), TEXT("pnc-multithread="), iMultithread))
        {
            bMultithread = iMultithread != 0;
        }
        int iRandAlloc;
        if (FParse::Value(FCommandLine::Get(), TEXT("pnc-randalloc="), iRandAlloc))
        {
            bRandAlloc = iRandAlloc != 0;
        }

        int iUseOOPVirtual;
        if (FParse::Value(FCommandLine::Get(), TEXT("pnc-oopv="), iUseOOPVirtual))
        {
            bUseOOPVirtual = iUseOOPVirtual != 0;
        }
        //const UCentipedeConfig* config = GetDefault<UCentipedeConfig>();
        CentipedeSegmentsRange = 0;
    }
    if (bUseConfigFile)
    {

        const UCentipedeConfig* config = GetDefault<UCentipedeConfig>();
        GConfig->GetInt(TEXT("/Script/UE5PNCDemo.CentipedeConfig"), TEXT("ChunkCount"), CentipedeChunkCount, "DefaultCentipedeConfig.ini");
        //CentipedeChunkCount = config->ChunkCount;
        CentipedeCountPerChunk = config->CentipedePerChunk;
        CentipedeSegmentsMin = config->SegmentPerCentipede;
        CentipedeSegmentsRange = 0;
        CentipedeSegmentLength = config->CentipedeSegmentLength;
    }

    InstancedMeshComponent = this->GetOwner()->GetComponentByClass<UInstancedStaticMeshComponent>();
    verifyf(InstancedMeshComponent, TEXT("Requires a UInstancedStaticMeshComponent component"));

    CentipedeBodyChunkStructure = pnc->GetOrAddChunkStructure<
            CoPosition,
            CoPositionPrevious,
            CoRotation,
            CoScale,
            CoVelocity,
            CoMass,
            CoSoftBodyNode,
            CoFInstancedStaticMeshInstanceData,
            CoCentipede,
            CoCentipedeBodyNode>();

    LegChunkStructure = pnc->GetOrAddChunkStructure<
            Ni::CoSingleParentOutsideChunk,
            Ni::CoParentInChunk,
            CoPosition,
            CoRotation,
            CoScale,
            CoLocalTransform,
            CoFInstancedStaticMeshInstanceData,
            CoCentipedeLegNode>();

    // Create all our centipedes
    for (int32 i = 0; i < CentipedeChunkCount; ++i)
        CreateCentipedes(CentipedeCountPerChunk);

    for (int i = 0; i < RandomAllocs.Num(); ++i)
    {
        FMemory::Free(RandomAllocs[i]);
    }
}

void UCentipedesPNC::CreateCentipedes(Ni::Size_t count)
{
    if (bUseOOP)
    {
        for (int i = 0; i < count; ++i)
        {
            DoRandomAlloc();
            int index = CentipedeObjects.Add(MakeUnique<FCentipede>());
            FCentipede * centipede = CentipedeObjects[index].Get();

            auto segementCountPerCentipete = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
            centipede->Init(this, segementCountPerCentipete);
            centipede->PropagateTransform();
        }
        return;
    }

    UPncAPI* pnc = UPncAPI::Get();
    if (!pnc)
        return;


    auto segementCountPerCentipete = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
    auto segementCountTotal = segementCountPerCentipete * count;
    DoRandomAlloc();
    auto& centipedeChunk = *pnc->NewChunkArray(CentipedeBodyChunkStructure, count, segementCountPerCentipete);
    ChunksCentipede.Add(centipedeChunk);
    CentipedeTickPipelines.Add( new CentipedePipeline(this));

    auto legNodesPerSegment = LegPerSegment * NodePerLeg * 2; // * 2 for left and right legs;
    auto totalLegNodes = segementCountPerCentipete * legNodesPerSegment;
    DoRandomAlloc();
    Ni::KArrayTree& legChunkArray = *pnc->NewChunkArray(LegChunkStructure, segementCountTotal, legNodesPerSegment);
    
    // attach legs chunk to the centipede body chunk
    centipedeChunk.InsertFirstChild(&legChunkArray);

    // Initialize the chunks data.
    InitCentipede(this).Run(centipedeChunk);
    CopyPreviousPosition().Run(centipedeChunk);

    // run the centipede logic once to move the centipede
    CentipedeLogic(this, 0).Run(centipedeChunk);

    // make sure all segment of the centipede are together at the correct length
    ConstrainSoftBodyPositions().Run(centipedeChunk);

    InitCentipedeLegs(segementCountPerCentipete, LegPerSegment, NodePerLeg, LegNodeLength, LegScale, LegScale2).Run(legChunkArray);
    
}

void UCentipedesPNC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Compute the total number of nodes that will need to be rendered and resize 
    // the InstancedMeshComponent accordingly.
    if(bRender)
    {
        PNC_PROFILE(TEXT("Centipede Reserve space"));
        int32 totalNodes = 0;
        if (bUseOOP)
        {
            for (const TUniquePtr<FCentipede>& c : CentipedeObjects)
                totalNodes += c->GetNodeCount();
        }
        else
        {
            for (auto cw : ChunksCentipede)
            {
                auto& c = cw.get();
                totalNodes += c.GetNodeCount();

                auto firstChild = c.GetFirstChildChunk();
                auto currentChild = firstChild;
                if (currentChild != nullptr)
                    do
                    {
                        totalNodes += currentChild->GetChunk().GetNodeCount();
                        currentChild = currentChild->GetNextSiblingChunk();
                    } while (currentChild != firstChild);

            }
        }
        CentipedeObjectNodeTransform.SetNum(totalNodes);
        auto availableCount = InstancedMeshComponent->GetInstanceCount();
        auto neededCount = totalNodes;
        while (availableCount < neededCount)
        {
            InstancedMeshComponent->AddInstance(FTransform::Identity);
            ++availableCount;
        }
        while (availableCount < neededCount)
            InstancedMeshComponent->RemoveInstance(availableCount--);
    }

    {

        if (bUseOOP)
        {
            PNC_PROFILE(TEXT("Centipede Update OOP"));
            if (bMultithread)
            {
                if (bUseOOPVirtual)
                {
                    ParallelFor(ObjectsIBeginFrame.Num(), [&](int32 Index) { ObjectsIBeginFrame[Index]->BeginFrame(this, DeltaTime); });
                    //ParallelFor(ObjectsIUpdate.Num(), [&](int32 Index) { ObjectsIUpdate[Index]->Update(this, DeltaTime); });
                    ParallelFor(ObjectsIUpdatePhysics.Num(), [&](int32 Index) { ObjectsIUpdatePhysics[Index]->UpdatePhysics(this, DeltaTime); });
                    ParallelFor(ObjectsIUpdateAnimation.Num(), [&](int32 Index) { ObjectsIUpdateAnimation[Index]->UpdateAnimation(this, DeltaTime); });
                    ParallelFor(ObjectsIPropagateTransform.Num(), [&](int32 Index) { ObjectsIPropagateTransform[Index]->PropagateTransform(); });
                }
                else
                {
                    ParallelFor(CentipedeObjects.Num(), [&](int32 Index)
                    {
                        CentipedeObjects[Index]->Update(this, DeltaTime);
                        CentipedeObjects[Index]->PropagateTransform();
                    });
                }
            }
            else
            {
                if (bUseOOPVirtual)
                {

                    for (IBeginFrame* o : ObjectsIBeginFrame)
                        o->BeginFrame(this, DeltaTime);
                    //for (IUpdate* o : ObjectsIUpdate)
                    //    o->Update(this, DeltaTime);
                    for (IUpdatePhysics* o : ObjectsIUpdatePhysics)
                        o->UpdatePhysics(this, DeltaTime);
                    for (IUpdateAnimation* o : ObjectsIUpdateAnimation)
                        o->UpdateAnimation(this, DeltaTime);
                    for (IPropagateTransform* o : ObjectsIPropagateTransform)
                        o->PropagateTransform();
                }
                else 
                {
                    for (const TUniquePtr<FCentipede>& c : CentipedeObjects)
                    {
                        c->Update(this, DeltaTime);
                        c->PropagateTransform();
                    }
                }
            }

        }
        else 
        {
            PNC_PROFILE(TEXT("Centipede Update"));
            if (bMultithread)
            {
                // Run our pipelines on all the centipede Chunks.
                ParallelFor(
                    ChunksCentipede.Num(),
                    [&](int32 Index)
                    {
                        CentipedeTickPipelines[Index]->DeltaTime = DeltaTime;
                        CentipedeTickPipelines[Index]->Run(ChunksCentipede[Index].get());
                    }
                );
            }
            else
            {
                for (int i = 0; i < ChunksCentipede.Num(); ++i)
                {
                    CentipedeTickPipelines[i]->DeltaTime = DeltaTime;
                    CentipedeTickPipelines[i]->Run(ChunksCentipede[i].get());
                }
            }
        }
    }

    if (bRender)
    {
        // Update the InstancedMeshComponent from our Chunks CoFInstancedStaticMeshInstanceData component
        int32 currentNode = 0;

        if (bUseOOP)
        {
            int32 iNode=0;
            for (const TUniquePtr<FCentipede>& c : CentipedeObjects)
            {
                if (iNode > CentipedeObjectNodeTransform.Num())
                    break;
                iNode = c->SetNodeTransform(CentipedeObjectNodeTransform.GetData(), iNode);
            }
            InstancedMeshComponent->BatchUpdateInstancesData(0, iNode, CentipedeObjectNodeTransform.GetData());
        }
        else
        {
            for (auto cw : ChunksCentipede)
            {
                auto& c = cw.get();
                // get the instanced data from the chunk and copy it to our UInstancedStaticMeshComponent
                CoFInstancedStaticMeshInstanceData* data = c.GetComponentData<CoFInstancedStaticMeshInstanceData>();
                {
                    PNC_PROFILE(TEXT("Centipede BatchUpdateInstancesData"));
                    InstancedMeshComponent->BatchUpdateInstancesData(currentNode, c.GetNodeCount(), data);
                }
                currentNode += c.GetNodeCount();

                auto firstChild = c.GetFirstChildChunk();
                auto currentChild = firstChild;
                if (currentChild != nullptr)
                    do
                    {
                        auto& chunk = currentChild->GetChunk();
                        CoFInstancedStaticMeshInstanceData* dataChild = chunk.GetComponentData<CoFInstancedStaticMeshInstanceData>();
                        InstancedMeshComponent->BatchUpdateInstancesData(currentNode, chunk.GetNodeCount(), dataChild);
                        currentNode += chunk.GetNodeCount();
                        currentChild = currentChild->GetNextSiblingChunk();
                    } while (currentChild != firstChild);
            }
        }
    }
}

UCentipedesPNC::~UCentipedesPNC()
{
    for (auto p : CentipedeTickPipelines)
        delete p;
}


void UCentipedesPNC::DoRandomAlloc()
{
    if (!bRandAlloc)
        return;
    for (int i = 0; i < 20; ++i)
    {
        RandomAllocs.Add(FMemory::Malloc(sizeof(FLegNode)));
        RandomAllocs.Add(FMemory::Malloc(sizeof(FBodyNode)));
        RandomAllocs.Add(FMemory::Malloc(sizeof(FBodyNode)));
        RandomAllocs.Add(FMemory::Malloc(sizeof(FCentipede)));
        RandomAllocs.Add(FMemory::Malloc(FMath::RandRange(8, 128)));
    }
}
void FLegNode::PropagateTransform(const FTransform& root)
{
    FTransform worldTransform = LocalTransform.Value * root;
    SetWorldTransform(worldTransform);
    if (ChildNode.Get())
    {
        return ChildNode->PropagateTransform(worldTransform);
    }
}

void FBodyNode::PropagateTransform()
{
    FTransform worldTransform = FTransform(Rotation.Rotation, Position.Position, Scale.Scale);
    for (const TUniquePtr<FLegNode>& l : RootLegNodes)
        l->PropagateTransform(worldTransform);

    if (NextBodyNode.Get())
        NextBodyNode->PropagateTransform();
}