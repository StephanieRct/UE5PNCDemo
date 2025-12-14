#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will compute the world matrix for each SoftBodyNode in a chunk.
/// </summary>
struct ComputeSoftBodyInstancedMatirx : public Ni::Algorithm<ComputeSoftBodyInstancedMatirx>
{
    CoPosition* Position;
    CoRotation* Rotation;
    CoScale* Scale;
    CoSoftBodyNode* SoftBodyNode;
    CoFInstancedStaticMeshInstanceData* InstancedStaticMeshInstanceData;

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Position)) return false;
        if (!req.Component(Rotation)) return false;
        if (!req.Component(Scale)) return false;
        if (!req.Component(SoftBodyNode)) return false;
        if (!req.Component(InstancedStaticMeshInstanceData)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        ALGO_PROFILE(TEXT("ComputeSoftBodyInstancedMatirx"));
        FVector previousPosition;
        for (int i = 0; i < count; ++i)
        {
            auto currentPosition = Position[i].Position;
            auto forward = i == 0 ? Position[0].Position - Position[1].Position
                                         : previousPosition - currentPosition;
            float l = forward.Length();
            if (l < 0.001f)
                forward = FVector(-1, 0, 0);
            else
                forward /= l;
            auto up = FVector(0, 0, 1);
            if (FVector::DotProduct(forward, up) >= 0.998f)
                up = SoftBodyNode[i].Up;
            auto right = FVector::CrossProduct(up, forward);
            up = FVector::CrossProduct(forward, right);
            up.Normalize();
            right.Normalize();
            SoftBodyNode[i].Up = up;
            auto matrix = FMatrix(forward, right, up, currentPosition);
            FTransform trf = FTransform(matrix);
            Rotation[i].Rotation = trf.GetRotation();
            Scale[i].Scale = trf.GetScale3D();
            InstancedStaticMeshInstanceData[i].Transform = matrix;
            previousPosition = currentPosition;
        }
    }
};