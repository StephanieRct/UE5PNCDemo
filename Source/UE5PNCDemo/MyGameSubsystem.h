// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UE5PNC/public/Ni.h"
#include "UE5PNC/public/Registry.h"

#include "MyGameSubsystem.generated.h"


/**
 * 
 */
UCLASS()
class UE5PNCDEMO_API UMyGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
    Ni::ComponentTypeRegistry ComponentTypeRegistry;
    Ni::ChunkStructureRegistry ChunkStructureRegistry;
    Ni::ChunkTreeRegistry ChunkRegistry;

    // Add a component type we can use for our centipede chunks
    template<typename T>
    const Ni::ComponentType* GetOrAddComponentType() { return ComponentTypeRegistry.GetOrAddComponentType<T>(); }

    // Add a chunk structure from a list of component type
    const Ni::ChunkStructure* GetOrAddChunkStructure(const std::initializer_list<const Ni::ComponentType*>& aComponents)
    { return ChunkStructureRegistry.GetOrAddChunkStructure(aComponents); }

    // Add a chunk with a given chunk structure and capacity.
    Ni::KChunkTree* NewChunk(const Ni::ChunkStructure*const chunkStructure, const size_t nodeCount)
    { return ChunkRegistry.NewChunk(chunkStructure, Ni::PropNodeCount(nodeCount)); }

    // Add a chunk with a given chunk structure and capacity.
    Ni::KArrayTree* NewChunkArray(const Ni::ChunkStructure*const chunkStructure, const size_t chunkCount, const size_t nodeCountPerChunk)
    { return ChunkRegistry.NewChunkArray(chunkStructure, Ni::PropChunkCount(chunkCount), Ni::PropNodeCountPerChunk(nodeCountPerChunk)); }

};
