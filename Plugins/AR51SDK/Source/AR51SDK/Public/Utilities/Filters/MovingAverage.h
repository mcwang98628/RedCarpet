#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"

class AR51SDK_API TMovingAverage : public IFilterInterface
{
public:
    TMovingAverage(int32 Capacity) : Buffer(Capacity), Sum(0.0f) {}

    /// <summary>
    /// Adds a new measurement to the buffer and returns the new average.
    /// </summary>
    /// <param name="NewValue">The new measurement to add.</param>
    /// <returns>The new average after adding the measurement.</returns>
    float Filter(float NewValue)
    {
        if (Buffer.Num() == Buffer.Max())
        {
            Sum -= Buffer[0];
            Buffer.RemoveAt(0);
        }

        Buffer.Add(NewValue);
        Sum += NewValue;

        return Sum / Buffer.Num();
    }

private:
    TArray<float> Buffer;
    float Sum;
};
