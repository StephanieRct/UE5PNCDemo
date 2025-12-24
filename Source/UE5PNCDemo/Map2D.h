// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "public/PncMeshAPI.h"

#include "Map2D.generated.h"


struct CoMap : public Ni::ChunkComponent
{
public:
	//std::unordered_map<FIntVector, Ni::Size_t, std::hash<FIntVector>> ChunkPositionToIndex;
	NiT::std_unordered_map<int, Ni::Size_t> ChunkPositionToIndex;
	CoMap()
	{
		UE_LOG(LogTemp, Warning, TEXT("CoMap::Ctor"));
	}
};
struct CoMapCell : public Ni::NodeComponent
{
public:
	Ni::NChunkPointer* Chunk;

	Ni::ComponentPointer<CoMultiSectionMesh> Mesh;
};


struct CoCell : public Ni::ChunkComponent
{
public:
	//Top-Left corner position
	FIntVector2 TilePosition;
	//FVector WorldPosition;
};
struct CoTile : public Ni::NodeComponent
{
public:
	uint32 TileIndex;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5PNCDEMO_API UMap2D : public USceneComponent
{
	GENERATED_BODY()
#pragma region Static
private:
	static const Ni::ChunkStructure* MapChunkStructure;
	static const Ni::ChunkStructure* CellChunkStructure;
public:
	static const Ni::ChunkStructure* GetMapChunkStructure()
	{
		if (MapChunkStructure) return MapChunkStructure;
		return MapChunkStructure = UPncAPI::Get()->GetOrAddChunkStructure(
			UPncAPI::Get()->GetOrAddComponentType<CoMap>(),
			UPncAPI::Get()->GetOrAddComponentType<CoMapCell>()
		);
	}
	static const Ni::ChunkStructure* GetCellChunkStructure()
	{
		if (CellChunkStructure) return CellChunkStructure;
		return CellChunkStructure = UPncAPI::Get()->GetOrAddChunkStructure(
			UPncAPI::Get()->GetOrAddComponentType<CoCell>(),
			UPncAPI::Get()->GetOrAddComponentType<CoTile>()
		);
	}
#pragma endregion

public:

	// We will be using an procedural mesh to render the map on screen.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProceduralMeshComponent* ProceduralMeshComponent;

	// We will be using an procedural mesh to render the map on screen.
	UPROPERTY(EditAnywhere)
	UMaterialInstance* TileMaterial;

	UPROPERTY(EditAnywhere)
	uint32 MapChunkSideSize = 8;

	UPROPERTY(EditAnywhere)
	FIntVector2 TileCount = FIntVector2(8,8);

	TArray<TUniquePtr< MeshChunk>> MeshChunks;

	Ni::NChunkPointer Map;
public:	
	// Sets default values for this component's properties
	UMap2D();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CreateMap();
	void CreateCell(FIntVector position);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
