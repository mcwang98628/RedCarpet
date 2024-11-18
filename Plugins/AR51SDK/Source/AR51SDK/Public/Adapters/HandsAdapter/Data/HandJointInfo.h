#pragma once
#include <CoreMinimal.h>
#include "HandJointType.h"
#include "HandType.h"

/// <summary>
/// Provides information regarding a particular joint in a tracked hand.
/// Note that on platform where finger-based confidence is not available
/// the overall hand confidence will be used. Joints with a confidence of
/// zero are considered to be missing.
/// </summary>
struct AR51SDK_API HandJointInfo
{
public:
    EHandType HandType;
    EHandJointType JointType;
    float Confidence;
    FVector Position;
    FQuat Rotation;

    FVector LocalPosition;
    FQuat LocalRotation;

public:

    HandJointInfo(EHandType handType, EHandJointType jointType)
    {
        HandType = handType;
        JointType = jointType;
        Confidence = 0.f;
        Position = FVector::ZeroVector;
        Rotation = FQuat::Identity;
    }

    HandJointInfo(EHandType handType, EHandJointType jointType, float confidence, const FVector& position, const FQuat& rotation)
    {
        HandType = handType;
        JointType = jointType;
        Confidence = confidence;
        Position = position;
        Rotation = rotation;
        LocalPosition = position;
        LocalRotation = rotation;
    }

    HandJointInfo(EHandType handType, EHandJointType jointType, float confidence,
        const FVector& localPosition, const FQuat& localRotation,
        const FVector& position, const FQuat& rotation)
    {
        HandType = handType;
        JointType = jointType;
        Confidence = confidence;
        LocalPosition = localPosition;
        LocalRotation = localRotation;
        
        Position = position;
        Rotation = rotation;
    }

public:
    static TArray<FVector> GetConfidentPositions(const TArray<HandJointInfo>& jointsInfo, float minimumConfidence);

    static TArray<FVector> GetPositions(const TArray<HandJointInfo>& jointsInfo);

    static bool HasConfidentPositions(const TArray<HandJointInfo>& jointsInfo, float minimumConfidence);
};