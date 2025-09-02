// Define a few PNC components we will use with our chunks.
#pragma once
#include "CoreMinimal.h"


/// <summary>
/// Position of a node in world space
/// </summary>
struct CoPosition
{
public:
    FVector Position;
};

/// <summary>
/// Velocity of a node in world space
/// </summary>
struct CoVelocity
{
public:
    FVector Velocity;
};

/// <summary>
/// Mass of a node
/// </summary>
struct CoMass
{
public:
    float Mass;
};

/// <summary>
/// A soft body node will keep a specific distance (Length) from it's previous node.
/// </summary>
struct CoSoftBodyNode 
{
public:
    FVector Up;
    float Length;
};

/// <summary>
/// A centipede chunk will have a single instance of this component and will be used to
/// control the behaviour of the centipede.
/// All soft body nodes in the chunk will follow the HeadPosition while the centipede moves
/// toward its target.
/// </summary>
struct CoCentipede 
{
public:
    FVector HeadPosition;
    FVector Target;
    float Speed;
    float Timer;
};