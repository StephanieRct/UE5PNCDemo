// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PncAPI.h"
#include "ProceduralMeshComponent.h"
#include "PncMeshAPI.generated.h"

using IndexChunk_t = Ni::NBucket;
using VertexChunk_t = Ni::NBucket;

struct CoMeshSection : public Ni::NodeComponent
{
public:
	IndexChunk_t Index;
	VertexChunk_t Vertex;
};

struct CoMultiSectionMesh : public Ni::ChunkComponent
{
public:
	const Ni::ChunkStructure* DefaultIndexStructure;
	const Ni::ChunkStructure* DefaultVertexStructure;
	CoMeshSection* MeshSection;

	CoMultiSectionMesh(
		const Ni::ChunkStructure* defaultIndexStructure,
		const Ni::ChunkStructure* defaultVertexStructure,
		CoMeshSection* meshSection)
		: DefaultIndexStructure(defaultIndexStructure)
		, DefaultVertexStructure(defaultVertexStructure)
		, MeshSection(meshSection)
	{
	}
};

struct CoPosition3 : public Ni::NodeComponent
{
public:
	FVector Position;
};

struct CoNormal3 : public Ni::NodeComponent
{
public:
	FVector Normal;
};

template<Ni::Size_t TChannel>
struct CoUV : public Ni::NodeComponent
{
public:
	FVector2D UV;
};

struct CoColor : public Ni::NodeComponent
{
public:
	FColor Color;
};
struct CoTangent3 : public Ni::NodeComponent
{
public:
	FProcMeshTangent Tangent;
};


template<typename TIndex>
struct CoIndex : public Ni::NodeComponent
{
public:
	TIndex Index;
};

using MeshIndexedType = int32;
using CoMeshIndex = CoIndex<MeshIndexedType>;

//struct MeshIndexChunk : public Ni::Chunk
//{
//public:
//	using Base_t = Ni::Chunk;
//public:
//	CoMeshIndex* MeshIndex;
//	MeshIndexChunk(const ChunkStructure_t* chunkStructure, Size_t nodeCapacity, Size_t nodeCount = 0)
//		: Base_t(chunkStructure, nodeCapacity, nodeCount)
//	{
//	}
//};
//struct MeshVertexChunk : public Ni::Chunk
//{
//public:
//	using Base_t = Ni::Chunk;
//
//public:
//	CoPosition3* Position3;
//	CoNormal3* Normal3;
//	CoUV<0>* UV0;
//	CoColor* Color;
//	CoTangent3* Tangent3;
//
//
//	MeshVertexChunk(const ChunkStructure_t* chunkStructure, Size_t nodeCapacity, Size_t nodeCount = 0)
//		: Base_t(chunkStructure, nodeCapacity, nodeCount)
//	{
//	}
//};

struct MeshChunk : public Ni::NBucket
{
public:
	using Base_t = Ni::NBucket;
	using Size_t = Ni::Size_t;
	using IndexChunk_t = Ni::NBucket;
	using VertexChunk_t = Ni::NBucket;

protected:
	CoMeshSection* MeshSection;

public:
	MeshChunk(
		const Ni::ChunkStructure* meshStructure,
		const Ni::ChunkStructure* indexStructure,
		const Ni::ChunkStructure* vertexStructure,
		const Ni::NodeCapacity sectionCapacity,
		const Ni::NodeCount sectionCount = Ni::NodeCount::V_0())
		: Base_t(meshStructure, sectionCapacity, sectionCount)
	{
		MeshSection = this->GetComponentData<CoMeshSection>();
		new(&GetMesh()) CoMultiSectionMesh(indexStructure, vertexStructure, MeshSection);

	}

	MeshChunk(const Size_t sectionCapacity, const Size_t sectionCount = 0);

	MeshChunk(const MeshChunk& other) = delete; // TODO
	MeshChunk(const MeshChunk&& other) = delete; // TODO
	MeshChunk& operator=(const MeshChunk& other) = delete; // TODO


	Size_t AddSection(const Size_t indexCapacity, const Size_t vertexCapacity, const Size_t indexCount = 0, const Size_t vertexCount = 0)
	{
		Size_t index = AddNode();
		CoMultiSectionMesh& mesh = GetMesh();
		CoMeshSection& section = mesh.MeshSection[index];
		section.Index = IndexChunk_t(mesh.DefaultIndexStructure, indexCapacity, indexCount);
		section.Vertex = VertexChunk_t(mesh.DefaultVertexStructure, vertexCapacity, vertexCount);
		return index;
	}
	const CoMeshSection& GetSection(const Size_t sectionIndex)const { return MeshSection[sectionIndex]; }
	CoMeshSection& GetSection(const Size_t sectionIndex) { return MeshSection[sectionIndex]; }

	IndexChunk_t& GetIndexChunk(CoMeshSection& section)
	{
		return section.Index;
	}
	VertexChunk_t& GetVertexChunk(CoMeshSection& section)
	{
		return section.Index;
	}

	//Ni::ChunkPointer& GetIndexChunk(const Size_t sectionIndex)
	//{
	//	CoMultiSectionMesh& mesh = GetMesh();
	//	CoMeshSection& section = mesh.MeshSection[sectionIndex];
	//	return *section.Index;
	//}

	//Ni::ChunkPointer& GetVertexChunk(const Size_t sectionIndex)
	//{
	//	CoMultiSectionMesh& mesh = GetMesh();
	//	CoMeshSection& section = mesh.MeshSection[sectionIndex];
	//	return *section.Vertex;
	//}



protected:
	CoMultiSectionMesh& GetMesh()
	{
		return *this->GetComponentData<CoMultiSectionMesh>();
	}
};


struct CreateTriangleVertices : public Ni::Algorithm<CreateTriangleVertices>
{
public:
	float Scale;
	CreateTriangleVertices(float scale = 100)
		: Scale(scale)
	{

	}

protected:
	CoPosition3* Position3;
	CoNormal3* Normal3;
	CoUV<0>* UV0;
	CoColor* Color;
	CoTangent3* Tangent3;

public:
	template<typename T>
	bool Requirements(T req)
	{
		if (!req.Component(Position3)) return false;
		if (!req.Component(Normal3)) return false;
		if (!req.Component(UV0)) return false;
		if (!req.Component(Color)) return false;
		if (!req.Component(Tangent3)) return false;
		return true;
	}

	void Execute(Ni::Size_t count)const
	{
		Position3[0].Position = FVector(-Scale, 0, 0);
		Position3[1].Position = FVector(Scale, 0, 0);
		Position3[2].Position = FVector(0, 0, Scale);


		Normal3[0].Normal = FVector(0, 1, 0);
		Normal3[1].Normal = FVector(0, 1, 0);
		Normal3[2].Normal = FVector(0, 1, 0);

		UV0[0].UV = FVector2D(0, 1);
		UV0[1].UV = FVector2D(1, 1);
		UV0[2].UV = FVector2D(0.5f, 0);

		Color[0].Color = FColor::Red;
		Color[1].Color = FColor::Green;
		Color[2].Color = FColor::Blue;

		Tangent3[0].Tangent = FProcMeshTangent();
		Tangent3[1].Tangent = FProcMeshTangent();
		Tangent3[2].Tangent = FProcMeshTangent();
		
	}

};



template<typename TIndex>
struct ConstructIndexIdentity : public Ni::Algorithm<ConstructIndexIdentity<TIndex>>
{
public:
	using Index_t = TIndex;

protected:
	CoIndex<Index_t>* Index;

public:
	template<typename T>
	bool Requirements(T req)
	{
		if (!req.Component(Index)) return false;
		return true;
	}

	void Execute(Ni::Size_t count)const
	{
		for (Ni::Size_t i = 0; i < count; ++i)
		{
			Index[i].Index = (Index_t)i;
		}
	}
};

template<typename TIndex>
struct ConstructIndexSequence : public Ni::Algorithm<ConstructIndexSequence<TIndex>>
{
public:
	using Index_t = TIndex;

public:
	Ni::Size_t FirstNodeIndex;
	Ni::Size_t NodeCount;
	Index_t FirstIndex;

protected:
	CoIndex<Index_t>* Index;

public:
	ConstructIndexSequence(const Ni::Size_t FirstNodeIndex, const Ni::Size_t NodeCount, const Index_t FirstIndex)
		: FirstNodeIndex(FirstNodeIndex)
		, NodeCount(NodeCount)
		, FirstIndex(FirstIndex)
	{
	}

public:
	template<typename T>
	bool Requirements(T req)
	{
		if (!req.Component(Index)) return false;
		return true;
	}

	void Execute(const Ni::Size_t count)const
	{
		for (Ni::Size_t i = 0; i < NodeCount; ++i)
		{
			Index[FirstNodeIndex + i].Index = FirstIndex + (Index_t)i;
		}
	}

};


/**
 * 
 */
UCLASS()
class UE5PNCDEMO_API UPncMeshAPI : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	static UPncMeshAPI* Get()
	{
		if (!GEngine) return nullptr;
		return GEngine->GetEngineSubsystem<UPncMeshAPI>();
	}

private:
	const Ni::ComponentType* ComponentTypeMultiSectionMesh;
	const Ni::ComponentType* ComponentTypeMeshSection;
	const Ni::ComponentType* ComponentTypePosition3;
	const Ni::ComponentType* ComponentTypeNormal3;
	const Ni::ComponentType* ComponentTypeUV0;
	const Ni::ComponentType* ComponentTypeUV1;
	const Ni::ComponentType* ComponentTypeUV2;
	const Ni::ComponentType* ComponentTypeUV3;
	const Ni::ComponentType* ComponentTypeColor;
	const Ni::ComponentType* ComponentTypeTangent3;
	const Ni::ComponentType* ComponentTypeMeshIndex;

	const Ni::ChunkStructure* DefaultStructureMesh;
	const Ni::ChunkStructure* DefaultStructureIndex;
	const Ni::ChunkStructure* DefaultStructureVertex;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	static void Upload(UProceduralMeshComponent& proceduralMeshComponent, const Ni::NChunkPointer& chunk);

	static void ConstructTriangleSection(CoMeshSection& section)
	{
		ConstructIndexIdentity<MeshIndexedType>().Run(section.Index);
		CreateTriangleVertices().Run(section.Vertex);
	}
	static void AddTriangleSection(MeshChunk& meshChunk)
	{
		auto sectionIndex = meshChunk.AddSection(3, 3, 3, 3);
		CoMeshSection* sections = meshChunk.GetComponentData<CoMeshSection>();
		ConstructTriangleSection(sections[sectionIndex]);
	}


	//static void AddTriangle(MeshChunk& mesh, const Ni::Size_t sectionIndex)
	//{
	//	CoMeshSection& section = mesh.GetSection(sectionIndex);
	//	auto& indexChunk = mesh.GetIndexChunk(section);
	//	auto& vertexChunk = mesh.GetVertexChunk(section);

	//	auto firstIndex = indexChunk.GetNodeCount();
	//	auto firstVertex = vertexChunk.GetNodeCount();

	//	//indexChunk->Add
	//	ConstructIndexSequence<MeshIndexedType>(firstIndex, 3, firstVertex).Run(section.Index);
	//	//auto sectionIndex = meshChunk.AddSection(3, 3, 3, 3);
	//	//CoMeshSection* sections = meshChunk.GetComponentData<CoMeshSection>();
	//	//ConstructTriangleSection(sections[sectionIndex]);
	//}
	const Ni::ChunkStructure* GetDefaultStructureMesh()const { return DefaultStructureMesh; }
	const Ni::ChunkStructure* GetDefaultStructureIndex()const { return DefaultStructureIndex; }
	const Ni::ChunkStructure* GetDefaultStructureVertex()const { return DefaultStructureVertex; }
};

#define assert_pnc_meshbuilder(cond) if(!(cond)) { ensure(cond); return *this;}
struct MeshBuilder 
{
public:
	using Size_t = Ni::Size_t;
	using Index_t = MeshIndexedType;
	using IndexChunk_t = MeshChunk::IndexChunk_t;
	using VertexChunk_t = MeshChunk::VertexChunk_t;
public:

	Size_t CurrentSectionIndex; // can be equald to Mesh->GetNodeCount() when currently at end
	Size_t CurrentIndexIndex; // can be equald to CurrentIndexChunk->GetNodeCount() when currently at end
	Size_t CurrentVertexIndex; // can be equald to CurrentVertexChunk->GetNodeCount() when currently at end

	MeshChunk* Mesh;

	CoMeshSection* CurrentSection;
	IndexChunk_t* CurrentIndexChunk;
	VertexChunk_t* CurrentVertexChunk;
	CoMeshIndex* CurrentMeshIndex;
	CoPosition3* CurrentPosition3;
	CoNormal3* CurrentNormal3;
	CoUV<0>* CurrentUV0;
	CoColor* CurrentColor;
	CoTangent3* CurrentTangent3;

	static const Size_t IndexPerTriangle = 3;
	static const Size_t VertexPerTriangle = 3;
	static const Size_t IndexPerQuad = 6;
	static const Size_t VertexPerQuad = 4;
	static const Size_t QuadPerCube = 6;
	static const Size_t IndexPerCube = IndexPerQuad * QuadPerCube;
	static const Size_t VertexPerCube = VertexPerQuad * QuadPerCube;

public:
	MeshBuilder(MeshChunk& mesh)
		: CurrentSectionIndex(0)
		, CurrentIndexIndex(0)
		, CurrentVertexIndex(0)
		, Mesh(&mesh)
		, CurrentSection(nullptr)
		, CurrentIndexChunk(nullptr)
		, CurrentVertexChunk(nullptr)
		, CurrentMeshIndex(nullptr)
		, CurrentPosition3(nullptr)
		, CurrentNormal3(nullptr)
		, CurrentUV0(nullptr)
		, CurrentColor(nullptr)
		, CurrentTangent3(nullptr)
	{
	}
	MeshBuilder(MeshChunk& mesh, const Size_t sectionIndex)
		: CurrentSectionIndex(0)
		, CurrentIndexIndex(0)
		, CurrentVertexIndex(0)
		, Mesh(&mesh)
		, CurrentSection(nullptr)
		, CurrentIndexChunk(nullptr)
		, CurrentVertexChunk(nullptr)
		, CurrentMeshIndex(nullptr)
		, CurrentPosition3(nullptr)
		, CurrentNormal3(nullptr)
		, CurrentUV0(nullptr)
		, CurrentColor(nullptr)
		, CurrentTangent3(nullptr)
	{
		Section(sectionIndex);
	}

public:
#pragma region Section ////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& AddSection(const Size_t indexCapacity, const Size_t vertexCapacity)
	{
		assert_pnc_meshbuilder(Mesh->AvailableNodes() >= 1);
		return Section(Mesh->AddSection(indexCapacity, vertexCapacity));
	}
	MeshBuilder& Section(const Size_t sectionIndex)
	{
		assert_pnc_meshbuilder(sectionIndex != -1);
		assert_pnc_meshbuilder(sectionIndex < Mesh->GetNodeCount());
		CurrentSectionIndex = sectionIndex;
		CurrentSection = &Mesh->GetSection(sectionIndex);
		CurrentIndexChunk = &CurrentSection->Index;
		CurrentVertexChunk = &CurrentSection->Vertex;
		CurrentMeshIndex = CurrentIndexChunk->GetComponentData<CoMeshIndex>();
		CurrentPosition3 = CurrentVertexChunk->GetComponentData<CoPosition3>();
		CurrentNormal3 = CurrentVertexChunk->GetComponentData <CoNormal3>();
		CurrentUV0 = CurrentVertexChunk->GetComponentData <CoUV<0>>();
		CurrentColor = CurrentVertexChunk->GetComponentData <CoColor>();
		CurrentTangent3 = CurrentVertexChunk->GetComponentData <CoTangent3>();
		CurrentIndexIndex = 0;
		CurrentVertexIndex = 0;
		return *this;
	}

	MeshBuilder& NextSection()
	{
		assert_pnc_meshbuilder(CurrentSectionIndex + 1 <= Mesh->GetNodeCount());
		++CurrentSectionIndex;
		return Section(CurrentSectionIndex);
	}
#pragma endregion 

#pragma region Index
	////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& IndexAt(const Size_t indexIndex)
	{
		assert_pnc_meshbuilder(indexIndex != -1);
		assert_pnc_meshbuilder(indexIndex < CurrentIndexChunk->GetNodeCount());
		CurrentIndexIndex = indexIndex;
		return *this;
	}

	MeshBuilder& NextIndex()
	{
		assert_pnc_meshbuilder(CurrentIndexIndex + 1 <= CurrentIndexChunk->GetNodeCount());
		++CurrentIndexIndex;
		return *this;
	}
	MeshBuilder& NextIndices(Ni::Size_t count)
	{
		assert_pnc_meshbuilder(CurrentIndexIndex + count <= CurrentIndexChunk->GetNodeCount());
		CurrentIndexIndex += count;
		return *this;
	}

	MeshBuilder& Index(const Index_t index, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentIndexIndex != -1);
		assert_pnc_meshbuilder(CurrentIndexIndex + offset < CurrentIndexChunk->GetNodeCount());
		CurrentMeshIndex[CurrentIndexIndex + offset].Index = index;
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& AddIndex()
	{
		assert_pnc_meshbuilder(CurrentIndexChunk->AvailableNodes() >= 1);
		return IndexAt(CurrentIndexChunk->AddNode());
	}

	MeshBuilder& AddIndex(const Index_t index)
	{
		return AddIndex().Index(index);
	}

	MeshBuilder& AddIndices(Ni::Size_t count)
	{
		assert_pnc_meshbuilder(CurrentIndexChunk->AvailableNodes() >= count);
		return IndexAt(CurrentIndexChunk->AddNodes(count));
	}

#pragma endregion

#pragma region Vertex

	////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& VertexAt(const Size_t vertexIndex)
	{
		assert_pnc_meshbuilder(vertexIndex != -1);
		assert_pnc_meshbuilder(vertexIndex < CurrentVertexChunk->GetNodeCount());
		CurrentVertexIndex = vertexIndex;
		return *this;
	}

	MeshBuilder& NextVertex()
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + 1 <= CurrentVertexChunk->GetNodeCount());
		++CurrentVertexIndex;
		return *this;
	}

	MeshBuilder& NextVertices(Ni::Size_t count)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + count <= CurrentVertexChunk->GetNodeCount());
		CurrentVertexIndex += count;
		return *this;
	}

	MeshBuilder& VertexDefault(Ni::Size_t offset = 0)
	{
		return Position(FVector(0, 0, 0), offset)
			.Normal(FVector(1, 0, 0), offset)
			.UV0(FVector2D(0, 0), offset)
			.Color(FColor::White, offset)
			.Tangent(FProcMeshTangent(), offset);
	}
	MeshBuilder& VerticesDefault(Ni::Size_t count, const Ni::Size_t offset = 0)
	{
		for (auto i = 0; i < count; ++i)
			VertexDefault(offset + i);
		return *this;
	}

	MeshBuilder& Position(const FVector value, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex != -1);
		assert_pnc_meshbuilder(CurrentVertexIndex + offset < CurrentVertexChunk->GetNodeCount());
		CurrentPosition3[CurrentVertexIndex + offset].Position = value;
		return *this;
	}

	MeshBuilder& Normal(const FVector value, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex != -1);
		assert_pnc_meshbuilder(CurrentVertexIndex + offset < CurrentVertexChunk->GetNodeCount());
		CurrentNormal3[CurrentVertexIndex + offset].Normal = value;
		return *this;
	}

	MeshBuilder& UV0(const FVector2D value, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex != -1);
		assert_pnc_meshbuilder(CurrentVertexIndex + offset < CurrentVertexChunk->GetNodeCount());
		CurrentUV0[CurrentVertexIndex + offset].UV = value;
		return *this;
	}

	MeshBuilder& Color(const FColor value, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex != -1);
		assert_pnc_meshbuilder(CurrentVertexIndex + offset < CurrentVertexChunk->GetNodeCount());
		CurrentColor[CurrentVertexIndex + offset].Color = value;
		return *this;
	}

	MeshBuilder& Tangent(const FProcMeshTangent value, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex != -1);
		assert_pnc_meshbuilder(CurrentVertexIndex + offset < CurrentVertexChunk->GetNodeCount());
		CurrentTangent3[CurrentVertexIndex + offset].Tangent = value;
		return *this;
	}

	////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& AddVertex()
	{
		assert_pnc_meshbuilder(CurrentVertexChunk->AvailableNodes() >= 1);
		return VertexAt(CurrentVertexChunk->AddNode());
	}

	MeshBuilder& AddVertexDefault()
	{
		assert_pnc_meshbuilder(CurrentVertexChunk->AvailableNodes() >= 1);
		return VertexAt(CurrentVertexChunk->AddNode())
			.VertexDefault();
	}

	MeshBuilder& AddVertices(Ni::Size_t count)
	{
		assert_pnc_meshbuilder(CurrentVertexChunk->AvailableNodes() >= count);
		return VertexAt(CurrentVertexChunk->AddNodes(count));
	}

	MeshBuilder& AddVerticesDefault(Ni::Size_t count)
	{
		assert_pnc_meshbuilder(CurrentVertexChunk->AvailableNodes() >= count);
		return VertexAt(CurrentVertexChunk->AddNodes(count))
			.VerticesDefault(count);
	}
	////////////////////////////////////////////////////////////////////////////////

#pragma endregion

#pragma region Triangle Index ////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& IndexTriangle(const Index_t vertexIndex0, const Index_t vertexIndex1, const Index_t vertexIndex2, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentIndexIndex + IndexPerTriangle <= CurrentIndexChunk->GetNodeCount());
		Index(vertexIndex0, offset);
		Index(vertexIndex1, offset + 1);
		Index(vertexIndex2, offset + 2);
		return *this;
	}

	MeshBuilder& NextIndexTriangle() { return NextIndices(IndexPerTriangle); }

	MeshBuilder& AddIndexTriangle(const Index_t vertexIndex0, const Index_t vertexIndex1, const Index_t vertexIndex2)
	{
		return AddIndices(IndexPerTriangle).IndexTriangle(vertexIndex0, vertexIndex1, vertexIndex2).NextIndexTriangle();
	}
#pragma endregion

#pragma region Triangle Vertex
#pragma endregion

#pragma region Quad Index ////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// </summary>
	/// <param name="vertexIndex0">bottom-left : x-, y-</param>
	/// <param name="vertexIndex1">bottom-right: x+, y-</param>
	/// <param name="vertexIndex2">top-left    : x-, y+</param>
	/// <param name="vertexIndex3">top-right   : x+, y+</param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& IndexQuad(const Index_t vertexIndex0, const Index_t vertexIndex1, const Index_t vertexIndex2, const Index_t vertexIndex3, const Ni::Size_t offset = 0)
	{
		IndexTriangle(vertexIndex0, vertexIndex1, vertexIndex2, offset);
		IndexTriangle(vertexIndex2, vertexIndex1, vertexIndex3, offset + IndexPerTriangle);
		return *this;
	}
	MeshBuilder& IndexQuad(const Index_t vertexIndexFirst, const Ni::Size_t offset = 0)
	{
		return IndexQuad(vertexIndexFirst, vertexIndexFirst +1, vertexIndexFirst +2, vertexIndexFirst +3, offset);
	}

	MeshBuilder& NextIndexQuad() { return NextIndices(IndexPerQuad); }

	MeshBuilder& AddIndexQuad(const Index_t vertexIndex0, const Index_t vertexIndex1, const Index_t vertexIndex2, const Index_t vertexIndex3)
	{
		return AddIndices(IndexPerQuad).IndexQuad(vertexIndex0, vertexIndex1, vertexIndex2, vertexIndex3).NextIndexQuad();
	}
	MeshBuilder& AddIndexQuad(const Index_t vertexIndexFirst)
	{
		return AddIndices(IndexPerQuad).IndexQuad(vertexIndexFirst).NextIndexQuad();
	}
#pragma endregion
	
#pragma region Quad Vertex ////////////////////////////////////////////////////////////////////////////////
	/// <summary>
	/// </summary>
	/// <param name="extent2">(x,y) -> (0, x, y)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadBack(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset+1;
		auto i2 = offset+2;
		auto i3 = offset+3;
		VertexDefault(i0).Position(FVector(0, -extent2.X, -extent2.Y) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(-1, 0, 0), i0).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i0);
		VertexDefault(i1).Position(FVector(0,  extent2.X, -extent2.Y) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(-1, 0, 0), i1).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i1);
		VertexDefault(i2).Position(FVector(0, -extent2.X,  extent2.Y) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(-1, 0, 0), i2).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i2);
		VertexDefault(i3).Position(FVector(0,  extent2.X,  extent2.Y) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(-1, 0, 0), i3).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i3);
		return *this;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="extent2">(x,y) -> (0, -x, y)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadFront(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset+1;
		auto i2 = offset+2;
		auto i3 = offset+3;
		VertexDefault(i0).Position(FVector(0,  extent2.X, -extent2.Y) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(1, 0, 0), i0).Tangent(FProcMeshTangent(FVector(0, -1, 0), false), i0);
		VertexDefault(i1).Position(FVector(0, -extent2.X, -extent2.Y) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(1, 0, 0), i1).Tangent(FProcMeshTangent(FVector(0, -1, 0), false), i1);
		VertexDefault(i2).Position(FVector(0,  extent2.X,  extent2.Y) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(1, 0, 0), i2).Tangent(FProcMeshTangent(FVector(0, -1, 0), false), i2);
		VertexDefault(i3).Position(FVector(0, -extent2.X,  extent2.Y) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(1, 0, 0), i3).Tangent(FProcMeshTangent(FVector(0, -1, 0), false), i3);
		return *this;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="extent2">(x,y) -> (-x, 0, y)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadLeft(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset + 1;
		auto i2 = offset + 2;
		auto i3 = offset + 3;
		VertexDefault(i0).Position(FVector( extent2.X, 0, -extent2.Y) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(0, -1, 0), i0).Tangent(FProcMeshTangent(FVector(-1, 0, 0), false), i0);
		VertexDefault(i1).Position(FVector(-extent2.X, 0, -extent2.Y) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(0, -1, 0), i1).Tangent(FProcMeshTangent(FVector(-1, 0, 0), false), i1);
		VertexDefault(i2).Position(FVector( extent2.X, 0,  extent2.Y) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(0, -1, 0), i2).Tangent(FProcMeshTangent(FVector(-1, 0, 0), false), i2);
		VertexDefault(i3).Position(FVector(-extent2.X, 0,  extent2.Y) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(0, -1, 0), i3).Tangent(FProcMeshTangent(FVector(-1, 0, 0), false), i3);
		return *this;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="extent2">(x,y) -> (x, 0, y)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadRight(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset + 1;
		auto i2 = offset + 2;
		auto i3 = offset + 3;
		VertexDefault(i0).Position(FVector(-extent2.X, 0, -extent2.Y) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(0, 1, 0), i0).Tangent(FProcMeshTangent(FVector(1, 0, 0), false), i0);
		VertexDefault(i1).Position(FVector( extent2.X, 0, -extent2.Y) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(0, 1, 0), i1).Tangent(FProcMeshTangent(FVector(1, 0, 0), false), i1);
		VertexDefault(i2).Position(FVector(-extent2.X, 0,  extent2.Y) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(0, 1, 0), i2).Tangent(FProcMeshTangent(FVector(1, 0, 0), false), i2);
		VertexDefault(i3).Position(FVector( extent2.X, 0,  extent2.Y) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(0, 1, 0), i3).Tangent(FProcMeshTangent(FVector(1, 0, 0), false), i3);
		return *this;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="extent2">(x,y) -> (-y, x, 0)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadDown(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset+1;
		auto i2 = offset+2;
		auto i3 = offset+3;
		VertexDefault(i0).Position(FVector( extent2.Y, -extent2.X, 0) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(0, 0, -1), i0).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i0);
		VertexDefault(i1).Position(FVector( extent2.Y,  extent2.X, 0) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(0, 0, -1), i1).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i1);
		VertexDefault(i2).Position(FVector(-extent2.Y, -extent2.X, 0) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(0, 0, -1), i2).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i2);
		VertexDefault(i3).Position(FVector(-extent2.Y,  extent2.X, 0) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(0, 0, -1), i3).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i3);
		return *this;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="extent2">(x,y) -> (y, x, 0)</param>
	/// <param name="center3"></param>
	/// <param name="offset"></param>
	/// <returns></returns>
	MeshBuilder& VertexQuadUp(const FVector2D extent2, const FVector center3, const Ni::Size_t offset = 0)
	{
		assert_pnc_meshbuilder(CurrentVertexIndex + VertexPerQuad <= CurrentVertexChunk->GetNodeCount());
		auto i0 = offset;
		auto i1 = offset+1;
		auto i2 = offset+2;
		auto i3 = offset+3;
		VertexDefault(i0).Position(FVector(-extent2.Y, -extent2.X, 0) + center3, i0).UV0(FVector2D(0, 1), i0).Normal(FVector(0, 0, 1), i0).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i0);
		VertexDefault(i1).Position(FVector(-extent2.Y,  extent2.X, 0) + center3, i1).UV0(FVector2D(1, 1), i1).Normal(FVector(0, 0, 1), i1).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i1);
		VertexDefault(i2).Position(FVector( extent2.Y, -extent2.X, 0) + center3, i2).UV0(FVector2D(0, 0), i2).Normal(FVector(0, 0, 1), i2).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i2);
		VertexDefault(i3).Position(FVector( extent2.Y,  extent2.X, 0) + center3, i3).UV0(FVector2D(1, 0), i3).Normal(FVector(0, 0, 1), i3).Tangent(FProcMeshTangent(FVector(0, 1, 0), false), i3);
		return *this;
	}
	
	MeshBuilder& NextVertexQuad()
	{
		return NextVertices(VertexPerQuad);
	}
	MeshBuilder& AddVertexQuadBack(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadBack(extent2, center3).NextVertexQuad();
	}
	MeshBuilder& AddVertexQuadFront(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadFront(extent2, center3).NextVertexQuad();
	}
	MeshBuilder& AddVertexQuadLeft(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadLeft(extent2, center3).NextVertexQuad();
	}
	MeshBuilder& AddVertexQuadRight(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadRight(extent2, center3).NextVertexQuad();
	}
	MeshBuilder& AddVertexQuadDown(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadDown(extent2, center3).NextVertexQuad();
	}
	MeshBuilder& AddVertexQuadUp(const FVector2D extent2, const FVector center3)
	{
		return AddVertices(VertexPerQuad).VertexQuadUp(extent2, center3).NextVertexQuad();
	}
#pragma endregion

#pragma region Quad ////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& QuadBack(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadBack(extent2, center3, vertexOffset);
	}
	MeshBuilder& QuadFront(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadFront(extent2, center3, vertexOffset);
	}
	MeshBuilder& QuadLeft(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadLeft(extent2, center3, vertexOffset);
	}
	MeshBuilder& QuadRight(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadRight(extent2, center3, vertexOffset);
	}
	MeshBuilder& QuadDown(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadDown(extent2, center3, vertexOffset);
	}
	MeshBuilder& QuadUp(const FVector2D extent2, const FVector center3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexQuad(CurrentVertexIndex, indexOffset).VertexQuadUp(extent2, center3, vertexOffset);
	}
	MeshBuilder& NextQuad()
	{
		return NextIndexQuad().NextVertexQuad();
	}
	MeshBuilder& AddQuadBack(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadBack(extent2, center3);
	}
	MeshBuilder& AddQuadFront(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadFront(extent2, center3);
	}
	MeshBuilder& AddQuadLeft(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadLeft(extent2, center3);
	}
	MeshBuilder& AddQuadRight(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadRight(extent2, center3);
	}
	MeshBuilder& AddQuadDown(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadDown(extent2, center3);
	}
	MeshBuilder& AddQuadUp(const FVector2D extent2, const FVector center3)
	{
		return AddIndexQuad(CurrentVertexIndex).AddVertexQuadUp(extent2, center3);
	}
#pragma endregion


#pragma region Cube Index ////////////////////////////////////////////////////////////////////////////////

	MeshBuilder& IndexCube(const Index_t vertexIndexFirst, const Ni::Size_t offset = 0)
	{
		for(auto i = 0; i < QuadPerCube; ++i)
			IndexQuad(vertexIndexFirst + VertexPerQuad * i, offset + IndexPerQuad * i);
		return *this;
	}
	MeshBuilder& NextIndexCube() { return NextIndices(IndexPerCube); }
	MeshBuilder& AddIndexCube(const Index_t vertexIndexFirst)
	{
		return AddIndices(IndexPerCube).IndexCube(vertexIndexFirst).NextIndexCube();
	}
#pragma endregion
#pragma region Cube Vertex ////////////////////////////////////////////////////////////////////////////////

	MeshBuilder& VertexCube(const FVector center3, const FVector extent3, const Ni::Size_t offset = 0)
	{
		VertexQuadBack (FVector2D(extent3.Y, extent3.Z), center3 - FVector(extent3.X, 0, 0), offset);
		VertexQuadFront(FVector2D(extent3.Y, extent3.Z), center3 + FVector(extent3.X, 0, 0), offset + VertexPerQuad);
		VertexQuadLeft (FVector2D(extent3.X, extent3.Z), center3 - FVector(0, extent3.Y, 0), offset + VertexPerQuad * 2);
		VertexQuadRight(FVector2D(extent3.X, extent3.Z), center3 + FVector(0, extent3.Y, 0), offset + VertexPerQuad * 3);
		VertexQuadDown (FVector2D(extent3.Y, extent3.X), center3 - FVector(0, 0, extent3.Z), offset + VertexPerQuad * 4);
		VertexQuadUp   (FVector2D(extent3.Y, extent3.X), center3 + FVector(0, 0, extent3.Z), offset + VertexPerQuad * 5);
		return *this;
	}
	MeshBuilder& NextVertexCube()
	{
		return NextVertices(VertexPerCube);
	}
	MeshBuilder& AddVertexCube(const FVector center3, const FVector extent3)
	{
		return AddVertices(VertexPerCube).VertexCube(center3, extent3).NextVertexQuad();
	}
#pragma endregion
#pragma region Cube ////////////////////////////////////////////////////////////////////////////////
	MeshBuilder& Cube(const FVector center3, const FVector extent3, const Ni::Size_t indexOffset = 0, const Ni::Size_t vertexOffset = 0)
	{
		return IndexCube(CurrentVertexIndex, indexOffset).VertexCube(center3, extent3, vertexOffset);
	}
	MeshBuilder& NextCube()
	{
		return NextIndexCube().NextVertexCube();
	}
	MeshBuilder& AddCube(const FVector extent3, const FVector center3)
	{
		return AddIndexCube(CurrentVertexIndex).AddVertexCube(extent3, center3);
	}
#pragma endregion

};
//
//struct MeshChunkPointer : public Ni::ChunkPointer
//{
//public:
//	using Base_t = Ni::ChunkPointer;
//	using Size_t = Ni::Size_t;
//public:
//	MeshChunkPointer(
//		const Ni::ChunkStructure* meshStructure, 
//		const Ni::ChunkStructure* vertexStructure, 
//		const Ni::ChunkStructure* indexStructure, 
//		const Size_t sectionCount)
//		: Base_t(meshStructure, sectionCount)
//	{
//	}
//};

