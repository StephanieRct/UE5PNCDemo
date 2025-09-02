#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UE5PNC/public/PNCDefault.h"

#include "CentipedesPNC.generated.h"


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

    // How much force is used to keep the centipede bodies together using the soft body relax algorithm.
    // Lower values than 1 will make the centipede more stretchable. 
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1"))
    float ForceRatio = 1;

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

    // Zone where the centipedes can move into
    UPROPERTY(EditAnywhere)
    float MaxDeltaTimeStep = 1/60.0f;

private:
    
    // ChunkType for our centipede nodes.
    const PNC::ChunkType* CentipedeChunkType;

    // Keep an array of all our component types
    TArray<TUniquePtr<PNC::ComponentType>> ComponentTypes;
    
    // Keep an array of all our chunk types
    TArray<TUniquePtr<PNC::ChunkType>> ChunkTypes;

    // Keep an array of all our chunks
    TArray<TUniquePtr<PNC::Chunk>> Chunks;

    // Keep an array of pointers to all our centipede chunks
    TArray<PNC::Chunk*> ChunksCentipede;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Add a component type we can use for our centipede chunks
    template<typename T>
    PNC::ComponentType* AddComponentType(PNC::ComponentOwner owner) 
    {
        return ComponentTypes[ComponentTypes.Add(MakeUnique<PNC::ComponentType>((T*)nullptr, owner))].Get();
    }

    // Add a chunk type from a list of component type
    const PNC::ChunkType* AddChunkType(const std::initializer_list<const PNC::ComponentType*>& aComponents) 
    {
        return ChunkTypes[ChunkTypes.Add(MakeUnique<PNC::ChunkType>(aComponents))].Get();
    }

    // Add a chunk with a given chunk type and capacity.
    PNC::Chunk* AddChunk(const PNC::ChunkType* chunkType, size_t capacity, size_t size = 0) 
    {
        return Chunks[Chunks.Add(MakeUnique<PNC::Chunk>(chunkType, capacity, size))].Get();
    }

    // Create a centipede chunk.
    PNC::Chunk* CreateCentipede();
};
