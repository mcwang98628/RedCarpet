#pragma once

#include "CoreMinimal.h"
#include "BoneContainer.h"	
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "FBoneTransformNode.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct AR51SDK_API FBoneTransformNode : public FAnimNode_SkeletalControlBase
{
    GENERATED_BODY()

public:
    // Constructor
    FBoneTransformNode();

    // FAnimNode_SkeletalControlBase interface
    virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
    virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
    virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
};

