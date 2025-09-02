// Fill out your copyright notice in the Description page of Project Settings.
#include "CentipedesPNC.h"

#include "components/components.h"

#include "Algorithms/CentipedeLogic.h"
#include "Algorithms/ComputeSoftBodyInstancedMatirx.h"
#include "Algorithms/ConstrainSoftBodyPositions.h"
#include "Algorithms/InitCentipede.h"
#include "Algorithms/AddDragToVelocity.h"
#include "Algorithms/SoftBodyRelax.h"
#include "Algorithms/ApplyVelocityOnPosition.h"
#include "Algorithms/ApplyGravityOnVelocity.h"

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
    const PNC::ComponentType* typePosition = AddComponentType<CoPosition>(PNC::ComponentOwner_Node);
    const PNC::ComponentType* typeVelocity = AddComponentType<CoVelocity>(PNC::ComponentOwner_Node);
    const PNC::ComponentType* typeMass = AddComponentType<CoMass>(PNC::ComponentOwner_Node);
    const PNC::ComponentType* typeFInstancedStaticMeshInstanceData = AddComponentType<FInstancedStaticMeshInstanceData>(PNC::ComponentOwner_Node);
    const PNC::ComponentType* typeSoftBodyNode = AddComponentType<CoSoftBodyNode>(PNC::ComponentOwner_Node);
    
    // this component is owned by the chunk meaning there's only 1 instance per chunk rather than 1 instance per node in the chunk.
    const PNC::ComponentType* typeCentipede = AddComponentType<CoCentipede>(PNC::ComponentOwner_Chunk);

    // Create a chunk type will all our components
    CentipedeChunkType = AddChunkType({
        typePosition,
        typeVelocity,
        typeMass,
        typeSoftBodyNode,
        typeFInstancedStaticMeshInstanceData,
        typeCentipede
        });

    // Create all our centipedes
    for (int32 i = 0; i < CentipedeCount; ++i)
        CreateCentipede();
}

PNC::Chunk* UCentipedesPNC::CreateCentipede()
{

    auto length = FMath::RandRange(CentipedeSegmentsMin, CentipedeSegmentsMin + CentipedeSegmentsRange);
    PNC::Chunk* centipedeChunk = AddChunk(CentipedeChunkType, length, length);
    ChunksCentipede.Add(centipedeChunk);

    // Initialize the chunk's data.
    InitCentipede(this).Run(*centipedeChunk);

    // run the centipede logic once to move the centipede
    CentipedeLogic(this, 0).Run(*centipedeChunk);

    // make sure all segment of the centipede are together at the correct length
    ConstrainSoftBodyPositions().Run(*centipedeChunk);

    return centipedeChunk;
}

void UCentipedesPNC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    int32 totalNodes = 0;
    for (auto c : ChunksCentipede)
        totalNodes += c->GetSize();

    {
        // Make sure the number of instance in the UInstancedStaticMeshComponent is equal to
        // the total number of nodes from all chunks
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Centipede Reserve space"));
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

    auto dt = DeltaTime;
    while(dt > 0)
    {
        auto stepDeltaTime = FMath::Min(dt, MaxDeltaTimeStep);
        dt -= stepDeltaTime;
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Centipede Update"));
        int32 currentNode = 0;
        for (auto c : ChunksCentipede)
        {
            // update the chunk with all the algorithm our centipedes need.
            CentipedeLogic(this, stepDeltaTime).Run(*c);
            AddDragToVelocity(this, stepDeltaTime).Run(*c);
            ApplyGravityOnVelocity(stepDeltaTime * Gravity).Run(*c);
            RelaxSoftBody(this, stepDeltaTime).Run(*c);
            ApplyVelocityOnPosition(stepDeltaTime).Run(*c);
            ComputeSoftBodyInstancedMatirx().Run(*c);

            // get the instanced data from the chunk and copy it to our UInstancedStaticMeshComponent
            FInstancedStaticMeshInstanceData* data = c->GetComponentData<FInstancedStaticMeshInstanceData>();
            {
                TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Centipede BatchUpdateInstancesData"));
                InstancedMeshComponent->BatchUpdateInstancesData(currentNode, c->GetSize(), data);
            }
            currentNode += c->GetSize();
        }
    }
}
