#pragma once

#include <CoreMinimal.h>
#include "UObject/ObjectMacros.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h" // Include this for UEnum in unreal 5.4 plu

UENUM(BlueprintType)
enum class EHandJointType : uint8
{
    Wrist = 0 UMETA(DisplayName = "Wrist"),

    ThumbTrapezium UMETA(DisplayName = "ThumbTrapezium"),
    ThumbMetaCarpal UMETA(DisplayName = "ThumbMetaCarpal"),
    ThumbProximal UMETA(DisplayName = "ThumbProximal"),
    ThumbDistal UMETA(DisplayName = "ThumbDistal"),
    ThumbTip UMETA(DisplayName = "ThumbTip"),

    IndexProximal UMETA(DisplayName = "IndexProximal"),
    IndexIntermediate UMETA(DisplayName = "IndexIntermediate"),
    IndexDistal UMETA(DisplayName = "IndexDistal"),
    IndexTip UMETA(DisplayName = "IndexTip"),

    MiddleProximal UMETA(DisplayName = "MiddleProximal"),
    MiddleIntermediate UMETA(DisplayName = "MiddleIntermediate"),
    MiddleDistal UMETA(DisplayName = "MiddleDistal"),
    MiddleTip UMETA(DisplayName = "MiddleTip"),

    RingProximal UMETA(DisplayName = "RingProximal"),
    RingIntermediate UMETA(DisplayName = "RingIntermediate"),
    RingDistal UMETA(DisplayName = "RingDistal"),
    RingTip UMETA(DisplayName = "RingTip"),

    LittleMetaCarpal UMETA(DisplayName = "LittleMetaCarpal"),
    LittleProximal UMETA(DisplayName = "LittleProximal"),
    LittleIntermediate UMETA(DisplayName = "LittleIntermediate"),
    LittleDistal UMETA(DisplayName = "LittleDistal"),
    LittleTip UMETA(DisplayName = "LittleTip"),
};


class AR51SDK_API HandJointTypeHelper {
public:
    static const TArray<EHandJointType>& GetTypes();

    static bool IsTip(EHandJointType type);

    static const TArray<EHandJointType>& GetThumbSequence();

    static const TArray<EHandJointType>& GetIndexSequence();

    static const TArray<EHandJointType>& GetMiddleSequence();

    static const TArray<EHandJointType>& GetRingSequence();

    static const TArray<EHandJointType>& GetPinkySequence();

    static FString ToString(EHandJointType handJointType) {
        switch (handJointType) {
            case EHandJointType::Wrist:
                return TEXT("Wrist");
            case EHandJointType::ThumbTrapezium:
                return TEXT("ThumbTrapezium");
            case EHandJointType::ThumbMetaCarpal:
                return TEXT("ThumbMetaCarpal");
            case EHandJointType::ThumbProximal:
                return TEXT("ThumbProximal");
            case EHandJointType::ThumbDistal:
                return TEXT("ThumbDistal");
            case EHandJointType::ThumbTip:
                return TEXT("ThumbTip");
            case EHandJointType::IndexProximal:
                return TEXT("IndexProximal");
            case EHandJointType::IndexIntermediate:
                return TEXT("IndexIntermediate");
            case EHandJointType::IndexDistal:
                return TEXT("IndexDistal");
            case EHandJointType::IndexTip:
                return TEXT("IndexTip");
            case EHandJointType::MiddleProximal:
                return TEXT("MiddleProximal");
            case EHandJointType::MiddleIntermediate:
                return TEXT("MiddleIntermediate");
            case EHandJointType::MiddleDistal:
                return TEXT("MiddleDistal");
            case EHandJointType::MiddleTip:
                return TEXT("MiddleTip");
            case EHandJointType::RingProximal:
                return TEXT("RingProximal");
            case EHandJointType::RingIntermediate:
                return TEXT("RingIntermediate");
            case EHandJointType::RingDistal:
                return TEXT("RingDistal");
            case EHandJointType::RingTip:
                return TEXT("RingTip");
            case EHandJointType::LittleMetaCarpal:
                return TEXT("LittleMetaCarpal");
            case EHandJointType::LittleProximal:
                return TEXT("LittleProximal");
            case EHandJointType::LittleIntermediate:
                return TEXT("LittleIntermediate");
            case EHandJointType::LittleDistal:
                return TEXT("LittleDistal");
            case EHandJointType::LittleTip:
                return TEXT("LittleTip");
            default:
                return TEXT("Unknown");
        }
    }


};
