#include "FBoneTransformNode.h"
#include "AnimationRuntime.h"
#include "Utils.h"
#include "Debug.h"
#include <Components\PoseableMeshComponent.h>
#include "Animation/AnimInstanceProxy.h" // required to compile in UE4.27
#include <AR51Character.h>

FBoneTransformNode::FBoneTransformNode() {}

void FBoneTransformNode::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    if (!Output.AnimInstanceProxy) {
        LOG("Error: Failed getting AnimInstanceProxy");
        return;
    }

    USkeletalMeshComponent* OwningComponent = Output.AnimInstanceProxy->GetSkelMeshComponent();
    if (!OwningComponent) {
        LOG("Error: Failed getting OwningComponent");
        return;
    }

    AActor* OwningActor = OwningComponent->GetOwner();
    if (!OwningActor) {
        LOG("Error: Failed getting OwningActor");
        return;
    }

    UAR51Character* Character = OwningActor->FindComponentByClass<UAR51Character>();
    if (!Character) {
        LOG("Error: Failed getting UAR51Character");
        return;
    }

    TArray<FName> BoneNames;
    Character->GetBoneNames(BoneNames);

    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    OutBoneTransforms.Empty();

    for (int32 i = 0; i < BoneNames.Num(); i++)
    {
        auto SkeletonBoneIndex = FSkeletonPoseBoneIndex(i);
#if ENGINE_MAJOR_VERSION == 5
        auto CompactPoseBoneIndex = BoneContainer.GetCompactPoseIndexFromSkeletonPoseIndex(SkeletonBoneIndex);
#elif ENGINE_MAJOR_VERSION == 4
        // Use the appropriate method for Unreal Engine 4.27
        auto CompactPoseBoneIndex = BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(SkeletonBoneIndex.GetInt()));
#endif


        if (CompactPoseBoneIndex != INDEX_NONE)
        {
            const FTransform& BoneTransformInComponentSpace = Character->GetBoneTransformByName(BoneNames[i], EBoneSpaces::Type::ComponentSpace);
            OutBoneTransforms.Add(FBoneTransform(FCompactPoseBoneIndex(SkeletonBoneIndex.GetInt()), BoneTransformInComponentSpace));
        }
    }

    OutBoneTransforms.Sort(FCompareBoneTransformIndex());
}

bool FBoneTransformNode::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    return true;
}

void FBoneTransformNode::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
}
