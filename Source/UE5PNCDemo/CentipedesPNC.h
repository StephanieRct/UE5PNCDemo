#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UE5PNC/public/PNCDefault.h"

#include "CentipedesPNC.generated.h"

struct CentipedePipeline;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5PNCDEMO_API UCentipedesPNC : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCentipedesPNC();

    // We will be using an instanced mesh to render each node on screen.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* InstancedMeshComponent;

    // Number of centipede chunks to create.
    UPROPERTY(EditAnywhere) 
    int32 CentipedeCount = 1;

    // Length of a centipede chunk.
    UPROPERTY(EditAnywhere, meta = (ClampMin = "2"))
    int32 CentipedeSegmentsMin = 20;

    // Length of a centipede chunk.
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
    int32 CentipedeSegmentsRange = 10;

    // Length of a centipede segment in cm.
    UPROPERTY(EditAnywhere)
    float CentipedeSegmentLength = 300;

    UPROPERTY(EditAnywhere)
    uint32 LegPerSegment = 1;
    
    UPROPERTY(EditAnywhere)
    uint32 NodePerLeg = 2;
    
    UPROPERTY(EditAnywhere)
    float LegNodeLength = 220;
    
    UPROPERTY(EditAnywhere)
    FVector LegScale = FVector(0.4, 0.4, 0.4);

    // Minimum mass of a centipede.
    UPROPERTY(EditAnywhere)
    FVector2D CentipedeMassRange = FVector2D(1, 50);

    // Mass multiplier for each segment of a centipede.
    UPROPERTY(EditAnywhere)
    FVector2D CentipedeSegmentMassVariation = FVector2D(1, 2);

    // Global speed for all centipede movement.
    UPROPERTY(EditAnywhere) 
    float CentipedeLogicSpeed = 1;

    // Gravity vector
    UPROPERTY(EditAnywhere) 
    FVector Gravity = FVector(0, 0, -9000);

    // How much air drag
    UPROPERTY(EditAnywhere) 
    float DragQ = 0.001f;

    // Speed of movement chosen when a centipede move to a new target/
    UPROPERTY(EditAnywhere) 
    FVector2D RandomSpeedRange = FVector2D(100, 500);

    // Zone where the centipedes can move into
    UPROPERTY(EditAnywhere) 
    FVector RandomTargetRange = FVector(10000, 10000, 1000);

    // Speed of movement chosen when a centipede move to a new target/
    UPROPERTY(EditAnywhere)
    FVector2D RandomRetargetTime = FVector2D(0.5, 10);

    // Max delta time when stepping physics
    UPROPERTY(EditAnywhere)
    float MaxDeltaTimeStep = 1/60.0f;

    UPROPERTY(EditAnywhere)
    float LegSegmentFrequency = 0.1;

    UPROPERTY(EditAnywhere)
    float LegDistanceFrequency = 0.01;

    UPROPERTY(EditAnywhere)
    FVector2D LegNode0ZRotationMinMax = FVector2D(-15, 15);
    UPROPERTY(EditAnywhere)
    FVector2D LegNode0YRotationMinMax = FVector2D(-45, 0);

    UPROPERTY(EditAnywhere)
    FVector2D LegNode1YRotationMinMax = FVector2D(90, 0);

    UPROPERTY(EditAnywhere)
    FVector2D LegNode2YRotationMinMax = FVector2D(-90, 0);

private:

    // ChunkType for our centipede nodes.
    const PNC::ChunkType* CentipedeChunkType;

    const PNC::ChunkType* LegChunkTypeOld;
    const PNC::ChunkType* LegChunkType;

    // Keep an array of all our component types
    TArray<TUniquePtr<PNC::ComponentType>> ComponentTypes;
    
    // Keep an array of all our chunk types
    TArray<TUniquePtr<PNC::ChunkType>> ChunkTypes;

    // Keep an array of all our chunks
    TArray<TUniquePtr<PNC::KChunkTree>> Chunks;

    // Keep an array of all our chunkarrays
    TArray<TUniquePtr<PNC::KChunkArrayTree>> ChunkArrays;

    // Keep an array of pointers to all our centipede chunks
    TArray<std::reference_wrapper<PNC::KChunkTree>> ChunksCentipede;
    
    // Pipeline executed on all centipede chunks during TickComponent
    CentipedePipeline* TickPipeline;
    
protected:
	virtual void BeginPlay() override;

public:	
    ~UCentipedesPNC();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Add a component type we can use for our centipede chunks
    template<typename T>
    PNC::ComponentType* AddComponentType() 
    {
        return ComponentTypes[ComponentTypes.Add(MakeUnique<PNC::ComponentType>((T*)nullptr, T::Owner))].Get();
    }

    // Add a chunk type from a list of component type
    const PNC::ChunkType* AddChunkType(const std::initializer_list<const PNC::ComponentType*>& aComponents) 
    {
        return ChunkTypes[ChunkTypes.Add(MakeUnique<PNC::ChunkType>(aComponents))].Get();
    }

    // Add a chunk with a given chunk type and capacity.
    PNC::KChunkTree& AddChunk(const PNC::ChunkType* chunkType, size_t capacity, size_t size = 0)
    {
        return *Chunks[Chunks.Add(MakeUnique<PNC::KChunkTree>(chunkType, capacity, size))].Get();
    }

    // Add a chunk with a given chunk type and capacity.
    PNC::KChunkArrayTree& AddChunkArray(const PNC::ChunkType* chunkType, size_t nodeCapacityPerChunk, size_t chunkCapacity, size_t chunkCount = 0, size_t nodeCountPerChunk = 0)
    {
        return *ChunkArrays[ChunkArrays.Add(MakeUnique<PNC::KChunkArrayTree>(chunkType, nodeCapacityPerChunk, chunkCapacity, chunkCount, nodeCountPerChunk))].Get();
    }
    // Create a centipede chunk.
    PNC::KChunkTree& CreateCentipede();
};
