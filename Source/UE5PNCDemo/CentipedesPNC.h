#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UE5PNC/public/Ni.h"
#include "MyGameSubsystem.h"

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

    UPROPERTY(EditAnywhere)
    bool bSingleChunk = true;

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

    UPROPERTY(EditAnywhere)
    FVector LegScale2 = FVector(0.5, 0.5, 0.5);
    
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
    bool bRender = true;

private:

    // Keep an array of references to all our centipede chunks
    TArray<std::reference_wrapper<Ni::KChunkTreePointer>> ChunksCentipede;
    
    // ChunkStructure for our centipede body nodes.
    const Ni::ChunkStructure* CentipedeBodyChunkStructure;

    // ChunkStructure for our centipede legs nodes.
    const Ni::ChunkStructure* LegChunkStructure;

    // Pipeline executed on all centipede chunks during TickComponent
    CentipedePipeline* TickPipeline;
    
protected:
	virtual void BeginPlay() override;

    TArray<void*> RandomAllocs;

    UMyGameSubsystem* GetPnc()const
    {
        UMyGameSubsystem* pnc = this->GetOwner()->GetGameInstance()->GetSubsystem<UMyGameSubsystem>();
        if (!pnc)
        {
            UE_LOG(LogTemp, Warning, TEXT("null UMyGameSubsystem"));
        }
        return pnc;
    }
public:	
    ~UCentipedesPNC();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Create a centipede chunk.
    Ni::KChunkTreePointer& CreateCentipede();

    // Create a centipede chunk.
    Ni::KChunkTreePointer& CreateCentipedes(Ni::Size_t count);
};
