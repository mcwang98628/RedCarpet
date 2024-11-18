#include "AR51AnimGraphNode.h"

FText UAR51AnimGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{

    return FText::FromString(FString("AR51AnimGraphNode"));
    //return NSLOCTEXT("AR51AnimGraphNode", "NodeTitle", "AR51 Mocap Streaming Node");
}

FLinearColor UAR51AnimGraphNode::GetNodeTitleColor() const
{
    return FLinearColor(0.0f, 1.0f, 0.0f);  // Adjust color as needed
}

FString UAR51AnimGraphNode::GetNodeCategory() const
{
    return TEXT("AR51 Mocap Unleashed");
}

FText UAR51AnimGraphNode::GetTooltipText() const
{
    return FText::FromString(FString("Streams the pose of the skeleton from AR 51 motion capture markerless system"));
    //return NSLOCTEXT("AR51AnimGraphNode", "Tooltip", "Streams the pose of the skeleton from AR 51 motion capture markerless system.");
}

const FAnimNode_SkeletalControlBase* UAR51AnimGraphNode::GetNode() const
{
    return &BoneTransformNode;  
}
