#pragma once

#include "CoreMinimal.h"
#include "FilterInterface.h"

class AR51SDK_API TKalmanFilter1D : public IFilterInterface
{
public:
    TKalmanFilter1D() {}

    float Filter(float measurement) override { return 0; }
};
