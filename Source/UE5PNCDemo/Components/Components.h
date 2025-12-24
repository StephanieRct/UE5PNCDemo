// Define a few Ni components we will use with our chunks.
#pragma once
#include "CoreMinimal.h"
#include "UE5PNC/public/Components.h"
#include "ProceduralMeshComponent.h"

//#define ALGO_PROFILE(x) TRACE_CPUPROFILER_EVENT_SCOPE(x)
#define ALGO_PROFILE(x) 
#define PNC_PROFILE(x) TRACE_CPUPROFILER_EVENT_SCOPE(x)
//#define PNC_PROFILE(x) 

using Ni::Size_t;

/// <summary>
/// Position of a node in world space
/// </summary>
struct CoPosition : public Ni::NodeComponent
{
public:
    FVector Position;
    CoPosition()
    {
        UE_LOG(LogTemp, Warning, TEXT("CoPosition::Ctor"));
    }
};

/// <summary>
/// Previous frame position of a node in world space
/// </summary>
struct CoPositionPrevious : public Ni::NodeComponent
{
public:
    FVector Position;
};
/// <summary>
/// 3D Rotation of a node in world space
/// </summary>
struct CoRotation : public Ni::NodeComponent
{
public:
    FQuat Rotation;
};

/// <summary>
/// 3D Scale of a node in world space
/// </summary>
struct CoScale : public Ni::NodeComponent
{
public:
    FVector Scale;
};

struct CoLocalTransform : public Ni::NodeComponent
{
public:
    FTransform Value;
    
};


/// <summary>
/// Velocity of a node in world space
/// </summary>
struct CoVelocity : public Ni::NodeComponent
{
public:
    FVector Velocity;
};

/// <summary>
/// Mass of a node
/// </summary>
struct CoMass : public Ni::NodeComponent
{
public:
    float Mass;
};

/// <summary>
/// A soft body node will keep a specific distance (Length) from it's previous node.
/// </summary>
struct CoSoftBodyNode : public Ni::NodeComponent
{
public:
    FVector Up;
    float Length;
};

struct CoCentipedeBodyNode : public Ni::NodeComponent
{
public:
    float Displacement;
};

/// <summary>
/// A centipede chunk will have a single instance of this component and will be used to
/// control the behaviour of the centipede.
/// All soft body nodes in the chunk will follow the HeadPosition while the centipede moves
/// toward its target.
/// </summary>
struct CoCentipede : public Ni::ChunkComponent
{
public:
    FVector HeadPosition;
    FVector Target;
    float Speed;
    float Timer;
};

struct CoCentipedeLeg : public Ni::ChunkComponent
{
public:
};
struct CoCentipedeLegNode : public Ni::NodeComponent
{
public:
    /// <summary>
    /// Keep the index of the parent node (centipede segment) for a leg nodes in a chunk.
    /// </summary>
    int32 SegmentIndex; // TODO Remove
    int32 JointIndex;
    float Phase;
    FVector RotationEulerBase;
    bool Chirality;// true: right, false: left
};


struct CoFInstancedStaticMeshInstanceData : public FInstancedStaticMeshInstanceData, public Ni::NodeComponent
{

};


