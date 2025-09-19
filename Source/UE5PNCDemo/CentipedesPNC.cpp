#include "CentipedesPNC.h"
#include "components/components.h"
#include "Pipelines/CentipedePipeline.h"

UCentipedesPNC::UCentipedesPNC()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCentipedesPNC::BeginPlay()
{
    Super::BeginPlay();

    InstancedMeshComponent = this->GetOwner()->GetComponentByClass<UInstancedStaticMeshComponent>();
    verifyf(InstancedMeshComponent, TEXT("Requires a UInstancedStaticMeshComponent component"));

    // create all our component types
    const PNC::ComponentType* typePosition = AddComponentType<CoPosition>();
    const PNC::ComponentType* typeRotation = AddComponentType<CoRotation>();
    const PNC::ComponentType* typeScale = AddComponentType<CoScale>();
    const PNC::ComponentType* typeFInstancedStaticMeshInstanceData = AddComponentType<CoFInstancedStaticMeshInstanceData>();
    const PNC::ComponentType* typePositionPrevious = AddComponentType<CoPositionPrevious>();
    const PNC::ComponentType* typeVelocity = AddComponentType<CoVelocity>();
    const PNC::ComponentType* typeMass = AddComponentType<CoMass>();
    const PNC::ComponentType* typeSoftBodyNode = AddComponentType<CoSoftBodyNode>();
    const PNC::ComponentType* typeCentipede = AddComponentType<CoCentipede>();
    const PNC::ComponentType* typeCentipedeBodyNode = AddComponentType<CoCentipedeBodyNode>();
    
    CentipedeChunkType = AddChunkType({
        typePosition,
        typePositionPrevious,
        typeRotation,
        typeScale,
        typeVelocity,
        typeMass,
        typeSoftBodyNode,
        typeFInstancedStaticMeshInstanceData,
        typeCentipede,
        typeCentipedeBodyNode,
    });

    const PNC::ComponentType* typeLocalTransform = AddComponentType<CoLocalTransform>();
    const PNC::ComponentType* typeCentipedeLegNode = AddComponentType<CoCentipedeLegNode>();
    const PNC::ComponentType* typeSingleParentOutsideChunk = AddComponentType<PNC::CoSingleParentOutsideChunk>();
    const PNC::ComponentType* typeCoParentInChunk = AddComponentType<PNC::CoParentInChunk>();

    LegChunkType = AddChunkType({
        typeSingleParentOutsideChunk,
        typeCoParentInChunk,
        typePosition,
        typeRotation,
        typeScale,
        typeLocalTransform,
        typeFInstancedStaticMeshInstanceData,
        typeCentipedeLegNode,
        });

    // Create all our centipedes
    for (int32 i = 0; i < CentipedeCount; ++i)
        CreateCentipede();

    TickPipeline = new CentipedePipeline(this);
}

PNC::KChunkTree& UCentipedesPNC::CreateCentipede()
{
    auto segementCount = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
    PNC::KChunkTree& centipedeChunk = AddChunk(CentipedeChunkType, segementCount, segementCount);
    ChunksCentipede.Add(centipedeChunk);

    uint32 legNodesPerSegment = LegPerSegment * NodePerLeg * 2; // * 2 for left and right legs;
    uint32 totalLegNodes = segementCount * legNodesPerSegment;

    // Initialize the chunks data.
    InitCentipede(this).Run(centipedeChunk);
    CopyPreviousPosition().Run(centipedeChunk);

    // run the centipede logic once to move the centipede
    CentipedeLogic(this, 0).Run(centipedeChunk);

    // make sure all segment of the centipede are together at the correct length
    ConstrainSoftBodyPositions().Run(centipedeChunk);

    PNC::KChunkArrayTree& legChunkArray = AddChunkArray(LegChunkType, legNodesPerSegment, segementCount, segementCount, legNodesPerSegment);
    InitCentipedeLegs(segementCount, LegPerSegment, NodePerLeg, LegNodeLength, LegScale).Run(legChunkArray);

    // attach legs chunk to the centipede body chunk
    centipedeChunk.InsertFirstChild(&legChunkArray);

    return centipedeChunk;
}

void UCentipedesPNC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Compute the total number of nodes that will need to be rendered and resize 
    // the InstancedMeshComponent accordingly.
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Centipede Reserve space"));
        int32 totalNodes = 0;
        for (auto cw : ChunksCentipede)
        {
            auto& c = cw.get();
            totalNodes += c->GetNodeCount();

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

    // Run our pipeline on all the centipede Chunks.
    TickPipeline->DeltaTime = DeltaTime;
    for (auto cw : ChunksCentipede)
        TickPipeline->Run(cw.get());

    // Update the InstancedMeshComponent from our Chunks CoFInstancedStaticMeshInstanceData component
    int32 currentNode = 0;
    for (auto cw : ChunksCentipede)
    {
        auto& c = cw.get();
        // get the instanced data from the chunk and copy it to our UInstancedStaticMeshComponent
        CoFInstancedStaticMeshInstanceData* data = c->GetComponentData<CoFInstancedStaticMeshInstanceData>();
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Centipede BatchUpdateInstancesData"));
            InstancedMeshComponent->BatchUpdateInstancesData(currentNode, c->GetNodeCount(), data);
        }
        currentNode += c->GetNodeCount();

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

UCentipedesPNC::~UCentipedesPNC()
{
    delete TickPipeline;
}
