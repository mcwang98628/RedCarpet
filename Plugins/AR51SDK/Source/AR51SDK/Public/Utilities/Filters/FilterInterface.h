#pragma once

#include "CoreMinimal.h"

class IFilterInterface
{
public:
    /// <summary>
    /// Updates the estimated state based on the measurement.
    /// </summary>
    /// <param name="measurement">The current measurement of the system state.</param>
    /// <returns>The estimated state after correction.</returns>
    virtual float Filter(float measurement) = 0;
};
