#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

struct PropagateTransformInChunk : public PNC::Algorithm<PropagateTransformInChunk>
{
public:
    FTransform RootTransform;
    PropagateTransformInChunk()
        : RootTransform(FTransform::Identity)
    {

    }
    //PropagateTransformInChunk(const FTransform& rootTransform)
    //    : RootTransform(rootTransform)
    //{ 
    //}

protected:
    PNC::CoParentInChunk* ParentInChunk;
    CoLocalTransform* LocalTransform;
    CoPosition* Position;
    CoRotation* Rotation;
    CoScale* Scale;
    CoFInstancedStaticMeshInstanceData* pCoInstancedStaticMeshInstanceData;

public:
    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(ParentInChunk)) return false;
        if (!req.Component(LocalTransform)) return false;
        if (!req.Component(Position)) return false;
        if (!req.Component(Rotation)) return false;
        if (!req.Component(Scale)) return false;
        if (!req.Component(pCoInstancedStaticMeshInstanceData)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("PropagateTransformInChunk"));
        for (int i = 0; i < count; ++i)
        {
            auto parentIndex = ParentInChunk[i].Index;
            FTransform transform = parentIndex >= 0 
                                        ? GetWorldTransform(parentIndex)
                                        : RootTransform;
            transform = LocalTransform[i].Value * transform;
            SetWorldTransform(i, transform);
            
            pCoInstancedStaticMeshInstanceData[i].Transform = transform.ToMatrixWithScale();
        }
    }

    FTransform GetWorldTransform(int32 index)const
    {
        return FTransform(Rotation[index].Rotation, Position[index].Position, Scale[index].Scale);
    }

    void SetWorldTransform(int32 index, const FTransform& transform)const
    {
        Rotation[index].Rotation = transform.GetRotation();
        Position[index].Position = transform.GetTranslation();
        Scale[index].Scale = transform.GetScale3D();
    }
};


struct PropagateTransformFromParentChunk : public PNC::Algorithm<PropagateTransformFromParentChunk>
{
protected:
    PropagateTransformInChunk Base;
    PNC::CoSingleParentOutsideChunk* SingleParentOutsideChunk;
    CoPosition* ParentPosition;
    CoRotation* ParentRotation;
    CoScale* ParentScale;

public:
    template<typename T>
    bool Requirements(T req)
    {
        if (!req.Component(SingleParentOutsideChunk)) return false;
        if (!req.ParentComponent(ParentPosition)) return false;
        if (!req.ParentComponent(ParentRotation)) return false;
        if (!req.ParentComponent(ParentScale)) return false;
        if (!Base.Requirements(req)) return false;
        return true;
    }

    void Execute(int count)
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("PropagateTransformInChunk"));
        auto parentIndex = SingleParentOutsideChunk->Index;
        Base.RootTransform = FTransform(ParentRotation[parentIndex].Rotation, ParentPosition[parentIndex].Position, ParentScale[parentIndex].Scale);
        Base.Execute(count);
    }

};


struct CopyPreviousPosition : public PNC::Algorithm<CopyPreviousPosition>
{
protected:
    CoPosition* Position;
    CoPositionPrevious* PositionPrevious;

public:
    template<typename T>
    bool Requirements(T req)
    {
        if (!req.Component(Position)) return false;
        if (!req.Component(PositionPrevious)) return false;
        return true;
    }

    void Execute(int count)const
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("CopyPreviousPosition"));
        for (int i = 0; i < count; ++i)
            PositionPrevious[i].Position = Position[i].Position;
    }

};
