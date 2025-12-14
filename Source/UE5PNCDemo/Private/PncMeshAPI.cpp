// Fill out your copyright notice in the Description page of Project Settings.


#include "PncMeshAPI.h"

void UPncMeshAPI::Initialize(FSubsystemCollectionBase& Collection)
{
    UPncAPI* pnc = UPncAPI::Get();
    assert(pnc);

    ComponentTypeMultiSectionMesh = pnc->GetOrAddComponentType<CoMultiSectionMesh>();
    ComponentTypeMeshSection = pnc->GetOrAddComponentType<CoMeshSection>();
    ComponentTypePosition3 = pnc->GetOrAddComponentType<CoPosition3>();
    ComponentTypeNormal3 = pnc->GetOrAddComponentType<CoNormal3>();
    ComponentTypeUV0 = pnc->GetOrAddComponentType<CoUV<0>>();
    ComponentTypeUV1 = pnc->GetOrAddComponentType<CoUV<1>>();
    ComponentTypeUV2 = pnc->GetOrAddComponentType<CoUV<2>>();
    ComponentTypeUV3 = pnc->GetOrAddComponentType<CoUV<3>>();
    ComponentTypeColor = pnc->GetOrAddComponentType<CoColor>();
    ComponentTypeTangent3 = pnc->GetOrAddComponentType<CoTangent3>();
    ComponentTypeMeshIndex = pnc->GetOrAddComponentType<CoMeshIndex>();

    DefaultStructureMesh = pnc->GetOrAddChunkStructure({
        ComponentTypeMultiSectionMesh,
        ComponentTypeMeshSection
        });

    DefaultStructureVertex = pnc->GetOrAddChunkStructure({
        ComponentTypePosition3,
        ComponentTypeNormal3,
        ComponentTypeUV0,
        ComponentTypeUV1,
        ComponentTypeUV2,
        ComponentTypeUV3,
        ComponentTypeColor,
        ComponentTypeTangent3
        });

    DefaultStructureIndex = pnc->GetOrAddChunkStructure({
        ComponentTypeMeshIndex
        });
}

void UPncMeshAPI::Upload(UProceduralMeshComponent& proceduralMeshComponent, const Ni::NChunkPointer& chunk)
{
    proceduralMeshComponent.ClearAllMeshSections();
    const CoMeshSection* sections = chunk.GetComponentData<CoMeshSection>();
    for (Ni::Size_t i = 0; i < chunk.GetNodeCount(); ++i)
    {
        const Ni::NChunkPointer& indexChunk = sections[i].Index;
        int32 indexCount = (int32)indexChunk.GetNodeCount(); //TODO test size type conversion fits

        const Ni::NChunkPointer& vertexChunk = sections[i].Vertex;
        int32 vertexCount = (int32)vertexChunk.GetNodeCount(); //TODO test size type conversion fits
        const CoPosition3* position3 = vertexChunk.GetComponentData<CoPosition3>();
        const CoNormal3* normal3 = vertexChunk.GetComponentData<CoNormal3>();
        const CoUV<0>* uv0 = vertexChunk.GetComponentData<CoUV<0>>();
        const CoColor* color = vertexChunk.GetComponentData<CoColor>();
        const CoTangent3* tangent3 = vertexChunk.GetComponentData<CoTangent3>();


        const CoMeshIndex* triangle = indexChunk.GetComponentData<CoMeshIndex>();
        proceduralMeshComponent.CreateMeshSection(
            i, // SectionIndex
            TArray<FVector>((const FVector*)position3, vertexCount),
            TArray<int32>((const int32*)triangle, indexCount),
            TArray<FVector>((const FVector*)normal3, vertexCount),
            TArray<FVector2D>((const FVector2D*)uv0, vertexCount),
            TArray<FColor>((const FColor*)color, vertexCount),
            TArray<FProcMeshTangent>((const FProcMeshTangent*)tangent3, vertexCount),
            false // bCreateCollision
        );
    }
}


MeshChunk::MeshChunk(const Size_t sectionCapacity, const Size_t sectionCount)
    : Base_t(
        UPncMeshAPI::Get()->GetDefaultStructureMesh(), 
        sectionCapacity,
        sectionCount)
{
    MeshSection = this->GetComponentData<CoMeshSection>();
    CoMultiSectionMesh* mesh = &GetMesh();
    const auto * indexStructure = UPncMeshAPI::Get()->GetDefaultStructureIndex();
    const auto* vertexStructure = UPncMeshAPI::Get()->GetDefaultStructureVertex();
    auto* meshSection = this->GetComponentData<CoMeshSection>();
    new(mesh) CoMultiSectionMesh(indexStructure, vertexStructure, meshSection);
}