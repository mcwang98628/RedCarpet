#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "FBoneTransformNode.h"
#include "AR51AnimGraphNode.generated.h"

UCLASS()
class UAR51AnimGraphNode : public UAnimGraphNode_SkeletalControlBase
{
    GENERATED_BODY()

public:
    // Override properties or methods as needed

    FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    FLinearColor GetNodeTitleColor() const override;
    FString GetNodeCategory() const override;
    FText GetTooltipText() const override;
    virtual const FAnimNode_SkeletalControlBase* GetNode() const override;

    // Specific to FBoneTransformNode
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FBoneTransformNode BoneTransformNode;
};

