#pragma once
#include "UE5PNC/public/Algorithm.h"
#include "Components/components.h"

/// <summary>
/// Will add drag to the velocity of all nodes in the chunk
/// </summary>
struct AddDragToVelocity : public PNC::Algorithm<AddDragToVelocity>
{
public:
    UCentipedesPNC* UComponent;
    float DeltaTime;

    CoVelocity* Velocity;

    AddDragToVelocity(UCentipedesPNC* uComponent, float deltaTime)
        : UComponent(uComponent) 
        , DeltaTime(deltaTime)
    {
    }

    template<typename T>
    bool Requirements(T req) 
    {
        if (!req.Component(Velocity)) return false;
        return true;
    }

    void Execute(int count)const 
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("Drag"));
        for (int i = 0; i < count; ++i)
        {
            auto vel = Velocity[i].Velocity;
            auto speed = vel.Length();
            if (speed > 0.01f)
            {
                vel = vel / speed;
                auto drag = UComponent->DragQ * 0.5f * speed * speed;
                drag = FMath::Min(drag, speed);
                Velocity[i].Velocity -= drag * vel;
            }
        }
    }
};