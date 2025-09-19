// Define a few PNC components we will use with our chunks.
#pragma once
#include "CoreMinimal.h"
#include "UE5PNC/public/Components.h"

/// <summary>
/// Position of a node in world space
/// </summary>
struct CoPosition : public PNC::NodeComponent
{
public:
    FVector Position;
};

/// <summary>
/// Previous frame position of a node in world space
/// </summary>
struct CoPositionPrevious : public PNC::NodeComponent
{
public:
    FVector Position;
};
/// <summary>
/// 3D Rotation of a node in world space
/// </summary>
struct CoRotation : public PNC::NodeComponent
{
public:
    FQuat Rotation;
};

/// <summary>
/// 3D Scale of a node in world space
/// </summary>
struct CoScale : public PNC::NodeComponent
{
public:
    FVector Scale;
};

struct CoLocalTransform : public PNC::NodeComponent
{
public:
    FTransform Value;
    
};


/// <summary>
/// Velocity of a node in world space
/// </summary>
struct CoVelocity : public PNC::NodeComponent
{
public:
    FVector Velocity;
};

/// <summary>
/// Mass of a node
/// </summary>
struct CoMass : public PNC::NodeComponent
{
public:
    float Mass;
};

/// <summary>
/// A soft body node will keep a specific distance (Length) from it's previous node.
/// </summary>
struct CoSoftBodyNode : public PNC::NodeComponent
{
public:
    FVector Up;
    float Length;
};

struct CoCentipedeBodyNode : public PNC::NodeComponent
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
struct CoCentipede : public PNC::ChunkComponent
{
public:
    FVector HeadPosition;
    FVector Target;
    float Speed;
    float Timer;
};

struct CoCentipedeLeg : public PNC::ChunkComponent
{
public:
};
struct CoCentipedeLegNode : public PNC::NodeComponent
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


struct CoFInstancedStaticMeshInstanceData : public FInstancedStaticMeshInstanceData, public PNC::NodeComponent
{

};