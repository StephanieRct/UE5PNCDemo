// Define a few Ni components we will use with our chunks.
#pragma once
#include "CoreMinimal.h"
#include "UE5PNC/public/PNCDefault.h"


template<typename TPipeline>
struct PropagateTransformPipelineT : public Ni::Pipeline<TPipeline>
{
    template<typename TPipeline2>
    struct Rebind 
    {
        using R = PropagateTransformPipelineT<TPipeline2>;
    };
    Ni::AlgorithmCacheRouter<PropagateTransformFromParentChunk> PropagateTransformFromParentChunkRouter;

    PropagateTransformPipelineT() = default;
    template<typename... TArgs>
    PropagateTransformPipelineT(TArgs&&...args)
    {
    }

    template<typename T>
    bool Requirements(T req)
    {
        // If any algorithm matches, this pipeline is matching.
        bool bMatch = false;
        bMatch |= req.Algorithm(PropagateTransformFromParentChunkRouter);
        return bMatch;
    }

    template<typename TContainer>
    void Execute(TContainer& chunk)
    {
        PropagateTransformFromParentChunkRouter.TryRun(PropagateTransformFromParentChunk(), chunk);
    }
};

template<typename TBefore, typename TAfter, typename TPipeline, typename TBase>
struct DPipelineHooksT : public TBase::template Rebind<DPipelineHooksT<TBefore, TAfter, TPipeline, TBase>>::R
{
    using Base_t = typename TBase::template Rebind<DPipelineHooksT<TBefore, TAfter, TPipeline, TBase>>::R;
    TBefore Before;
    TAfter After;

    template<typename... TArgs>
    DPipelineHooksT(TBefore&& before, TAfter&& after, TArgs&&...args)
        : Base_t(args...)
        , Before(std::move(before))
        , After(std::move(after))
    {
    }

    template<typename... TArgs>
    DPipelineHooksT(TArgs&&...args)
        : Base_t(args...)
        , Before(args...)
        , After(args...)
    {
    }
    template<typename TContainer>
    void Execute(TContainer& chunk)
    {
        Before.Execute(chunk);
        Base_t::Execute(chunk);
        After.Execute(chunk);
    }
};
template<typename TBefore, typename TAfter, typename TBase>
struct DPipelineHooks : public DPipelineHooksT< TBefore, TAfter, DPipelineHooks< TBefore, TAfter, TBase>, TBase>
{
    using Base_t = DPipelineHooksT< TBefore, TAfter, DPipelineHooks< TBefore, TAfter, TBase>, TBase>;
    using Base_t::Base_t;
};
