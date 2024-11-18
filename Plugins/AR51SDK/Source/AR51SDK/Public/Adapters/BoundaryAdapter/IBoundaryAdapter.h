#pragma once

#include <CoreMinimal.h>

class AR51SDK_API IBoundaryAdapter
{
public:
    virtual FString GetBoundaryName() const;
    virtual TArray<FVector> GetPoints() const = 0;
};