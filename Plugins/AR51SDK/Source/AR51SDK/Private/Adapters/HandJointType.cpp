#include "HandJointType.h"

const TArray<EHandJointType>& HandJointTypeHelper::GetTypes() {
    static TArray<EHandJointType> allTypes =
    {
        EHandJointType::Wrist,

        EHandJointType::ThumbMetaCarpal, // and not ThumbTrapezium ?? for htc
        EHandJointType::ThumbMetaCarpal,
        EHandJointType::ThumbProximal,
        EHandJointType::ThumbDistal,
        EHandJointType::ThumbTip,

        EHandJointType::IndexProximal,
        EHandJointType::IndexIntermediate,
        EHandJointType::IndexDistal,
        EHandJointType::IndexTip,

        EHandJointType::MiddleProximal,
        EHandJointType::MiddleIntermediate,
        EHandJointType::MiddleDistal,
        EHandJointType::MiddleTip,

        EHandJointType::RingProximal,
        EHandJointType::RingIntermediate,
        EHandJointType::RingDistal,
        EHandJointType::RingTip,

        EHandJointType::LittleMetaCarpal,
        EHandJointType::LittleProximal,
        EHandJointType::LittleIntermediate,
        EHandJointType::LittleDistal,
        EHandJointType::LittleTip,
    };
    return allTypes;
}

bool HandJointTypeHelper::IsTip(EHandJointType type)
{
    switch (type)
    {
        case EHandJointType::ThumbTip:
        case EHandJointType::IndexTip:
        case EHandJointType::MiddleTip:
        case EHandJointType::RingTip:
        case EHandJointType::LittleTip:
            return true;
        default:
            return false;
    }
}

const TArray<EHandJointType>& HandJointTypeHelper::GetThumbSequence()
{
    static TArray<EHandJointType> seq =
    {
        EHandJointType::ThumbTrapezium,
        EHandJointType::ThumbMetaCarpal,
        EHandJointType::ThumbProximal,
        EHandJointType::ThumbDistal,
        EHandJointType::ThumbTip,
    };
    return seq;
}

const TArray<EHandJointType>& HandJointTypeHelper::GetIndexSequence()
{
    static TArray<EHandJointType> seq =
    {
        EHandJointType::IndexProximal,
        EHandJointType::IndexIntermediate,
        EHandJointType::IndexDistal,
        EHandJointType::IndexTip,
    };
    return seq;
}

const TArray<EHandJointType>& HandJointTypeHelper::GetMiddleSequence()
{
    static TArray<EHandJointType> seq =
    {
        EHandJointType::MiddleProximal,
        EHandJointType::MiddleIntermediate,
        EHandJointType::MiddleDistal,
        EHandJointType::MiddleTip,
    };
    return seq;
}

const TArray<EHandJointType>& HandJointTypeHelper::GetRingSequence()
{
    static TArray<EHandJointType> seq =
    {
        EHandJointType::RingProximal,
        EHandJointType::RingIntermediate,
        EHandJointType::RingDistal,
        EHandJointType::RingTip,
    };
    return seq;
}

const TArray<EHandJointType>& HandJointTypeHelper::GetPinkySequence()
{
    static TArray<EHandJointType> seq =
    {
        EHandJointType::LittleMetaCarpal,
        EHandJointType::LittleProximal,
        EHandJointType::LittleIntermediate,
        EHandJointType::LittleDistal,
        EHandJointType::LittleTip,
    };
    return seq;
}
