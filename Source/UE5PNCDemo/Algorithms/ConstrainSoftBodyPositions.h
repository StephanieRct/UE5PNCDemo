#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will move the position of each SoftBodyNode in a chunk
/// </summary>
struct ConstrainSoftBodyPositions : public PNC::Algorithm<ConstrainSoftBodyPositions>
{
    CoPosition* Position;
    CoSoftBodyNode* SoftBodyNode;

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Position)) return false;
        if (!req.Component(SoftBodyNode)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("ConstrainSoftBodyPositions"));
        FVector previousPosition = Position[0].Position;
        const float sectionLenght = 300;
        for (int i = 1; i < count; ++i)
        {
            auto currentPosition = Position[i].Position;
            auto towardPrevious = previousPosition - currentPosition;
            float l = towardPrevious.Length();
            if (l < 0.001f)
                towardPrevious = FVector(-1, 0, 0);
            else
                towardPrevious.Normalize();
            previousPosition = Position[i].Position = previousPosition - SoftBodyNode[i].Length * towardPrevious;
        }
    }
};