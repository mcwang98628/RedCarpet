#include "HandJointInfo.h"

TArray<FVector> HandJointInfo::GetConfidentPositions(const TArray<HandJointInfo>& jointsInfo, float minimumConfidence)
{
    return HasConfidentPositions(jointsInfo, minimumConfidence) ? GetPositions(jointsInfo) : TArray<FVector>();
}

TArray<FVector> HandJointInfo::GetPositions(const TArray<HandJointInfo>& jointsInfo)
{
    TArray<FVector> positions;
    positions.Reserve(jointsInfo.Num());
    for (auto i = 0; i < jointsInfo.Num(); i++)
        positions.Add(jointsInfo[i].Position);

    return positions;
}

bool HandJointInfo::HasConfidentPositions(const TArray<HandJointInfo>& jointsInfo, float minimumConfidence)
{
    return jointsInfo.Num() > 0 && jointsInfo[0].Confidence >= minimumConfidence;
}
