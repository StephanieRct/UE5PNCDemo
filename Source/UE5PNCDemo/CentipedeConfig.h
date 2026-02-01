
#pragma once
#include "UObject/NoExportTypes.h"
#include "CentipedeConfig.generated.h"

UCLASS(Config = CentipedeConfig, BlueprintType)
class UCentipedeConfig : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, BlueprintReadWrite, Category = Settings)
    int32 ChunkCount;
    UPROPERTY(Config, BlueprintReadWrite, Category = Settings)
    int32 CentipedePerChunk;

    UPROPERTY(Config, BlueprintReadWrite, Category = Settings)
    int32 SegmentPerCentipede;

    UPROPERTY(Config, BlueprintReadWrite, Category = Settings)
    float CentipedeSegmentLength = 300;
};