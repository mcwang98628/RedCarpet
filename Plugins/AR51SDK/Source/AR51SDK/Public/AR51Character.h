// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include <Components\PoseableMeshComponent.h>
#include <Components\SkinnedMeshComponent.h>
#include <Animation\NodeMappingContainer.h>
#include "AR51Character.generated.h"

class SkeletonInfo;
class HandIKSolver;
class BoneTransform;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UAR51Character : public UPoseableMeshComponent
{
	GENERATED_BODY()
private:
	FString _skeletonId;
	TMap<FName, FName> _nodeMapping;

	TSharedPtr<SkeletonInfo> _skeletonInfo;
	TSharedPtr<HandIKSolver> _handIKSolver;
	TArray< USceneComponent*> _controllers;
	TArray<TSharedPtr<BoneTransform>> _emptyJoints;
	
	TArray<TSharedPtr<BoneTransform>> _rightHandJoints;
	TArray<TSharedPtr<BoneTransform>> _leftHandJoints;


#ifndef UE_5_0_OR_LATER
	USkeletalMesh* GetSkinnedAsset() const { return SkeletalMesh; }
#endif

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UNodeMappingContainer* SkeletonMapping { nullptr };

public:	
	// Sets default values for this component's properties
	UAR51Character();

	FName GetBoneName(FName& name);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void Solve(const TArray<FVector>& positions);
	void SolveHands(const TArray<FVector>& lHandPos, const TArray<FVector>& rHandPos);
	void ApplyOnSkeletalMeshes();

	FVector GetHeadPosition();
	FQuat GetHeadRotation();

	FVector GetLeftWristPosition();
	FVector GetRightWristPosition();

	const TArray<TSharedPtr<BoneTransform>>& GetLeftHandJoints() const;
	const TArray<TSharedPtr<BoneTransform>>& GetRightHandJoints() const;

	void SetHeadRotation(const FQuat& rotation);

	UFUNCTION(BlueprintCallable, Category = "Components|PoseableMesh")
	void SetBoneLocalTransformByName(FName BoneName, const FTransform& InTransform);


	UFUNCTION(BlueprintCallable, Category = "Components|PoseableMesh")
	FTransform GetBoneLocalTransformByName(FName BoneName);

	FString GetSkeletonId() const { return _skeletonId;  }
	void SetSkeletonId(const FString& id) { _skeletonId = id; }

};
