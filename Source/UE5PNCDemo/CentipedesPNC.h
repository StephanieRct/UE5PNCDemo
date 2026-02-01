#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UE5PNC/public/Ni.h"
#include "MyGameSubsystem.h"

#include "components/components.h"

#include "CentipedesPNC.generated.h"
#define OOP_PAD(x) int x[8]

struct IBeginFrame
{
    virtual void BeginFrame(UCentipedesPNC* uComponent, float deltaTime) = 0;
    virtual ~IBeginFrame() {}
};
//struct IUpdate
//{
//    virtual void Update(UCentipedesPNC* uComponent, float deltaTime) = 0;
//    virtual ~IUpdate() {}
//};
struct IUpdatePhysics
{
    virtual void UpdatePhysics(UCentipedesPNC* uComponent, float deltaTime) = 0;
    virtual ~IUpdatePhysics() {}
};
struct IUpdateAnimation
{
    virtual void UpdateAnimation(UCentipedesPNC* uComponent, float deltaTime) = 0;
    virtual ~IUpdateAnimation() {}
};
struct IPropagateTransform
{
    virtual void PropagateTransform()=0;
    virtual ~IPropagateTransform() {}
};

struct CentipedePipeline;
struct FCentipede;
struct FBodyNode;
struct IObject
{
    virtual ~IObject() {}
};
struct FLegNode : public IObject, public IUpdateAnimation
{
    //FBodyNode* SingleParentOutsideChunk;
    //FLegNode* ParentInChunk;
    TUniquePtr<FLegNode> ChildNode;
    OOP_PAD(Pad0);
    CoPosition Position;
    OOP_PAD(Pad1);
    CoRotation Rotation;
    OOP_PAD(Pad2);
    CoScale Scale;
    OOP_PAD(Pad3);
    CoLocalTransform LocalTransform;
    OOP_PAD(Pad4);
    //CoFInstancedStaticMeshInstanceData FInstancedStaticMeshInstanceData;
    CoCentipedeLegNode CentipedeLegNode;
    OOP_PAD(Pad5);
    FBodyNode* BodyNode;

    virtual void UpdateAnimation(UCentipedesPNC* uComponent, float deltaTime) override;

    uint32 GetNodeCount()const
    {
        if (ChildNode.Get())
        {
            return ChildNode->GetNodeCount() + 1;
        }
        return 1;
    }


    uint32 SetNodeTransform(FInstancedStaticMeshInstanceData* data, uint32 index)const
    {
        data[index++].Transform = FTransform(Rotation.Rotation, Position.Position, Scale.Scale).ToMatrixWithScale();
        if (ChildNode.Get())
        {
            index = ChildNode->SetNodeTransform(data, index);
        }
        return index;
    }

    void SetWorldTransform(const FTransform& transform)
    {
        Rotation.Rotation = transform.GetRotation();
        Position.Position = transform.GetTranslation();
        Scale.Scale = transform.GetScale3D();
    }

    virtual void PropagateTransform(const FTransform& root);
};

struct FBodyNode : public IObject
{
    CoPosition Position;
    OOP_PAD(Pad0);
    CoPositionPrevious PositionPrevious;
    OOP_PAD(Pad1);
    CoRotation Rotation;
    OOP_PAD(Pad2);
    CoScale Scale;
    OOP_PAD(Pad3);
    CoVelocity Velocity;
    OOP_PAD(Pad4);
    CoMass Mass;
    OOP_PAD(Pad5);
    CoSoftBodyNode SoftBodyNode;
    OOP_PAD(Pad6);
    //CoFInstancedStaticMeshInstanceData InstancedStaticMeshInstanceData;
    CoCentipedeBodyNode CentipedeBodyNode;
    OOP_PAD(Pad7);
    TArray<TUniquePtr<FLegNode>> RootLegNodes;
    OOP_PAD(Pad8);
    TUniquePtr<FBodyNode> NextBodyNode;
    OOP_PAD(Pad9);

    FBodyNode* PreviousBodyNode;
    FCentipede* Centipede;
    //void Update(UCentipedesPNC* uComponent, float deltaTime);
    uint32 GetNodeCount()const
    {
        uint32 total = 0;
        for (const TUniquePtr<FLegNode>& l : RootLegNodes)
            total += l->GetNodeCount();
        if (NextBodyNode.Get())
            total += NextBodyNode->GetNodeCount();
        return total + 1;
    }

    void PropagateTransform();

    uint32 SetNodeTransform(FInstancedStaticMeshInstanceData* data, uint32 index)const
    {
        data[index++].Transform = FTransform(Rotation.Rotation, Position.Position, Scale.Scale).ToMatrixWithScale();
        for (const TUniquePtr<FLegNode>& l : RootLegNodes)
            index = l->SetNodeTransform(data, index);

        if (NextBodyNode.Get())
            index = NextBodyNode->SetNodeTransform(data, index);
        return index;
    }
};

struct FCentipede : public IObject, 
    public IPropagateTransform,
    public IBeginFrame,
    public IUpdatePhysics
    //public IUpdateAnimation
    //public IUpdate
{
    virtual void Init(UCentipedesPNC* uComponent, int segmentCount);
    virtual void BeginFrame(UCentipedesPNC* uComponent, float deltaTime);
    virtual void UpdatePhysics(UCentipedesPNC* uComponent, float deltaTime);
    //virtual void UpdateAnimation(UCentipedesPNC* uComponent, float deltaTime);
    
    
    virtual void Update(UCentipedesPNC* uComponent, float deltaTime);


    virtual void PropagateTransform() override
    {
        RootBodyNode->PropagateTransform();
    }

    uint32 GetNodeCount()const
    {
        return RootBodyNode->GetNodeCount();
    }
    uint32 SetNodeTransform(FInstancedStaticMeshInstanceData* data, uint32 index)const
    {
        return RootBodyNode->SetNodeTransform(data, index);
    }
    CoCentipede Centipede;
    OOP_PAD(Pad0);
    TUniquePtr<FBodyNode> RootBodyNode;
    OOP_PAD(Pad1);
    //TArray<TUniquePtr<FBodyNode>> BodyNodes;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5PNCDEMO_API UCentipedesPNC : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCentipedesPNC();

    // We will be using an instanced mesh to render each node on screen.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* InstancedMeshComponent;

    UPROPERTY(EditAnywhere)
    bool bUseOOP = false;

    UPROPERTY(EditAnywhere)
    bool bUseOOPVirtual = false;

    UPROPERTY(EditAnywhere)
    bool bMultithread = true;

    UPROPERTY(EditAnywhere)
    bool bRandAlloc = false;

    UPROPERTY(EditAnywhere)
    bool bUseConfigFile = false;

    UPROPERTY(EditAnywhere)
    bool bUseCommandLine = false;
    //
    //// Number of centipede chunks to create.
    //UPROPERTY(EditAnywhere) 
    //int32 CentipedeCount = 1;

    UPROPERTY(EditAnywhere)
    int32 CentipedeChunkCount = 1;

    UPROPERTY(EditAnywhere)
    int32 CentipedeCountPerChunk = 1;


    // Length of a centipede chunk.
    UPROPERTY(EditAnywhere, meta = (ClampMin = "2"))
    int32 CentipedeSegmentsMin = 20;

    // Length of a centipede chunk.
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
    int32 CentipedeSegmentsRange = 10;

    // Length of a centipede segment in cm.
    UPROPERTY(EditAnywhere)
    float CentipedeSegmentLength = 300;

    UPROPERTY(EditAnywhere)
    uint32 LegPerSegment = 1;
    
    UPROPERTY(EditAnywhere)
    uint32 NodePerLeg = 2;
    
    UPROPERTY(EditAnywhere)
    float LegNodeLength = 220;
    
    UPROPERTY(EditAnywhere)
    FVector LegScale = FVector(0.4, 0.4, 0.4);

    UPROPERTY(EditAnywhere)
    FVector LegScale2 = FVector(0.5, 0.5, 0.5);
    
    // Minimum mass of a centipede.
    UPROPERTY(EditAnywhere)
    FVector2D CentipedeMassRange = FVector2D(1, 50);

    // Mass multiplier for each segment of a centipede.
    UPROPERTY(EditAnywhere)
    FVector2D CentipedeSegmentMassVariation = FVector2D(1, 2);

    // Global speed for all centipede movement.
    UPROPERTY(EditAnywhere) 
    float CentipedeLogicSpeed = 1;

    // Gravity vector
    UPROPERTY(EditAnywhere) 
    FVector Gravity = FVector(0, 0, -9000);

    // How much air drag
    UPROPERTY(EditAnywhere) 
    float DragQ = 0.001f;

    // Speed of movement chosen when a centipede move to a new target/
    UPROPERTY(EditAnywhere) 
    FVector2D RandomSpeedRange = FVector2D(100, 500);

    // Zone where the centipedes can move into
    UPROPERTY(EditAnywhere) 
    FVector RandomTargetRange = FVector(10000, 10000, 1000);

    // Speed of movement chosen when a centipede move to a new target/
    UPROPERTY(EditAnywhere)
    FVector2D RandomRetargetTime = FVector2D(0.5, 10);

    // Max delta time when stepping physics
    UPROPERTY(EditAnywhere)
    float MaxDeltaTimeStep = 1/60.0f;

    UPROPERTY(EditAnywhere)
    float LegSegmentFrequency = 0.1;

    UPROPERTY(EditAnywhere)
    float LegDistanceFrequency = 0.01;

    UPROPERTY(EditAnywhere)
    FVector2D LegNode0ZRotationMinMax = FVector2D(-15, 15);
    UPROPERTY(EditAnywhere)
    FVector2D LegNode0YRotationMinMax = FVector2D(-45, 0);

    UPROPERTY(EditAnywhere)
    FVector2D LegNode1YRotationMinMax = FVector2D(90, 0);

    UPROPERTY(EditAnywhere)
    bool bRender = true;

    //template<typename T>
    //void AddObjectCallbacks(T* obj)
    //{
    //    if (IBeginFrame* pIBeginFrame = dynamic_cast<IBeginFrame*>(obj)) ObjectsIBeginFrame.Add(pIBeginFrame);
    //    if (IUpdate* pIUpdate = dynamic_cast<IUpdate*>(obj)) ObjectsIUpdate.Add(pIUpdate);
    //    if (IUpdatePhysics* pIUpdatePhysics = dynamic_cast<IUpdatePhysics*>(obj)) ObjectsIUpdatePhysics.Add(pIUpdatePhysics);
    //    if (IUpdateAnimation* pIUpdateAnimation = dynamic_cast<IUpdateAnimation*>(obj)) ObjectsIUpdateAnimation.Add(pIUpdateAnimation);
    //    if (IPropagateTransform* pIPropagateTransform = dynamic_cast<IPropagateTransform*>(obj)) ObjectsIPropagateTransform.Add(pIPropagateTransform);

    //}
    void AddObjectCallbacks(FCentipede* obj)
    {
        ObjectsIBeginFrame.Add(obj);
        ObjectsIUpdatePhysics.Add(obj);
        ObjectsIPropagateTransform.Add(obj);

    }
    void AddObjectCallbacks(FBodyNode* obj)
    {
    }
    void AddObjectCallbacks(FLegNode* obj)
    {
        ObjectsIUpdateAnimation.Add(obj);

    }
private:

    // Keep an array of references to all our centipede chunks
    TArray<std::reference_wrapper<Ni::KChunkTreePointer>> ChunksCentipede;
    TArray<CentipedePipeline*> CentipedeTickPipelines;

    // ChunkStructure for our centipede body nodes.
    const Ni::ChunkStructure* CentipedeBodyChunkStructure;

    // ChunkStructure for our centipede legs nodes.
    const Ni::ChunkStructure* LegChunkStructure;
    

    TArray<TUniquePtr<FCentipede>> CentipedeObjects;
    TArray<FInstancedStaticMeshInstanceData> CentipedeObjectNodeTransform;

    TArray<IBeginFrame*> ObjectsIBeginFrame;
    //TArray<IUpdate*> ObjectsIUpdate;
    TArray<IUpdatePhysics*> ObjectsIUpdatePhysics;
    TArray<IUpdateAnimation*> ObjectsIUpdateAnimation;
    TArray<IPropagateTransform*> ObjectsIPropagateTransform;


protected:
	virtual void BeginPlay() override;

    TArray<void*> RandomAllocs;

    UMyGameSubsystem* GetPnc()const
    {
        UMyGameSubsystem* pnc = this->GetOwner()->GetGameInstance()->GetSubsystem<UMyGameSubsystem>();
        if (!pnc)
        {
            UE_LOG(LogTemp, Warning, TEXT("null UMyGameSubsystem"));
        }
        return pnc;
    }
public:	
    ~UCentipedesPNC();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Create a centipede chunk.
    //Ni::KChunkTreePointer& CreateCentipede();

    // Create a centipede chunk.
    void CreateCentipedes(Ni::Size_t count);

    void DoRandomAlloc();
};


