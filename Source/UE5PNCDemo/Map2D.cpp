// Fill out your copyright notice in the Description page of Project Settings.


#include "Map2D.h"

const Ni::ChunkStructure* UMap2D::MapChunkStructure = nullptr;
const Ni::ChunkStructure* UMap2D::CellChunkStructure = nullptr;

// Sets default values for this component's properties
UMap2D::UMap2D()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	
}


// Called when the game starts
void UMap2D::BeginPlay()
{
	Super::BeginPlay();

	if(!ProceduralMeshComponent)
		ProceduralMeshComponent = this->GetOwner()->GetComponentByClass<UProceduralMeshComponent>();

	ni_assert(ProceduralMeshComponent);
	
	CreateMap();

	auto v = MakeUnique<MeshChunk>(1);
	MeshChunk& meshChunk = *v.Get();
	MeshChunks.Add(std::move(v));

	MeshBuilder builder(meshChunk);
	builder.AddSection(MeshBuilder::IndexPerCube, MeshBuilder::VertexPerCube);
	builder.AddCube(FVector(0, 0, 500), FVector(50, 100, 150));
	UPncMeshAPI::Upload(*ProceduralMeshComponent, meshChunk);

	ProceduralMeshComponent->SetMaterial(0, TileMaterial);
	
}


// Called every frame
void UMap2D::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UMap2D::CreateMap()
{
	UPncAPI* pnc = UPncAPI::Get();
	auto* map = pnc->NewChunk(GetMapChunkStructure(), 16);
}
void UMap2D::CreateCell(FIntVector position)
{
	UPncAPI* pnc = UPncAPI::Get();
	//pnc->NewChunk(GetMapChunkStructure(),)
}