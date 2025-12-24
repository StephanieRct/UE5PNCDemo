#include "CentipedesPNC.h"
#include "components/components.h"
#include "Pipelines/CentipedePipeline.h"
#include "UE5PNC/public/PncAPI.h"
#include "MyGameSubsystem.h"

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
    if (bSingleChunk)
    {
        CreateCentipedes(CentipedeCount);
    }
    else
    {
        for (int32 i = 0; i < CentipedeCount; ++i)
            CreateCentipede();
    }

    TickPipeline = new CentipedePipeline(this);

    for (int i = 0; i < RandomAllocs.Num(); ++i)
    {
        FMemory::Free(RandomAllocs[i]);
    }
}

Ni::KChunkTreePointer& UCentipedesPNC::CreateCentipede()
{
    UPncAPI* pnc = UPncAPI::Get();
    if (!pnc)
        return *(Ni::KChunkTreePointer*)nullptr;

    const int randomAllocCount = 1024;
    const int randomAllocMinSize = 8;
    const int randomAllocMaxSize = 1024;
    TArray<void*> randomAllocs;
    randomAllocs.Reserve(randomAllocCount);
    for (int i = 0; i < randomAllocCount; ++i)
    {
        randomAllocs.Add(FMemory::Malloc(FMath::RandRange(randomAllocMinSize, randomAllocMaxSize)));
        RandomAllocs.Add(FMemory::Malloc(FMath::RandRange(randomAllocMinSize, randomAllocMaxSize)));
    }

    auto segementCount = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
    Ni::KChunkTree& centipedeChunk = *pnc->NewChunk(CentipedeBodyChunkStructure, segementCount);
    ChunksCentipede.Add(centipedeChunk);

    uint32 legNodesPerSegment = LegPerSegment * NodePerLeg * 2; // * 2 for left and right legs;
    uint32 totalLegNodes = segementCount * legNodesPerSegment;
    Ni::KArrayTree& legChunkArray = *pnc->NewChunkArray(LegChunkStructure, segementCount, legNodesPerSegment);

    // attach legs chunk to the centipede body chunk
    centipedeChunk.InsertFirstChild(&legChunkArray);
    
    // Initialize the chunks data.
    InitCentipede(this).Run(centipedeChunk);
    CopyPreviousPosition().Run(centipedeChunk);

    // run the centipede logic once to move the centipede
    CentipedeLogic(this, 0).Run(centipedeChunk);

    // make sure all segment of the centipede are together at the correct length
    ConstrainSoftBodyPositions().Run(centipedeChunk);

    InitCentipedeLegs(segementCount, LegPerSegment, NodePerLeg, LegNodeLength, LegScale, LegScale2).Run(legChunkArray);

    for (int i = 0; i < randomAllocCount; ++i)
    {
        FMemory::Free(randomAllocs[i]);
    }

    return centipedeChunk;
}

Ni::KChunkTreePointer& UCentipedesPNC::CreateCentipedes(Ni::Size_t count)
{
    UPncAPI* pnc = UPncAPI::Get();
    if (!pnc)
        return *(Ni::KChunkTreePointer*)nullptr;

    auto segementCountPerCentipete = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
    auto segementCountTotal = segementCountPerCentipete * count;
    auto& centipedeChunk = *pnc->NewChunkArray(CentipedeBodyChunkStructure, count, segementCountPerCentipete);
    ChunksCentipede.Add(centipedeChunk);

    auto legNodesPerSegment = LegPerSegment * NodePerLeg * 2; // * 2 for left and right legs;
    auto totalLegNodes = segementCountPerCentipete * legNodesPerSegment;
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
    return centipedeChunk;
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
        PNC_PROFILE(TEXT("Centipede Update"));
        // Run our pipeline on all the centipede Chunks.
        TickPipeline->DeltaTime = DeltaTime;
        for (auto cw : ChunksCentipede)
            TickPipeline->Run(cw.get());
    }

    if (bRender)
    {
        // Update the InstancedMeshComponent from our Chunks CoFInstancedStaticMeshInstanceData component
        int32 currentNode = 0;
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

UCentipedesPNC::~UCentipedesPNC()
{
    delete TickPipeline;
}
