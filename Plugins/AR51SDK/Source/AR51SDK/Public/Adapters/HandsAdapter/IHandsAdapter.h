#pragma once

#include <CoreMinimal.h>
#include "HandJointInfo.h"

class AR51SDK_API IHandsAdapter
{
public:
    virtual FString GetSourceDevice() const { return "NoSource"; }
    virtual TArray<HandJointInfo> GetRightJointInfos() = 0;
    virtual TArray<HandJointInfo> GetLeftJointInfos() = 0;
};