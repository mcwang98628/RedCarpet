#pragma once

#include "CoreMinimal.h"
#include <Components\PoseableMeshComponent.h>
#include "Debug.h"

class AR51SDK_API BoneTransform {
private:
	UPoseableMeshComponent* _poseableMesh{ nullptr };

	TSharedPtr<BoneTransform> _parent;
	TArray<TSharedPtr<BoneTransform>> _children;

	FVector _initLocalPosition;
	FQuat _initLocalRotation;
	FVector _initLocalScale;

	FVector _initPosition;
	FQuat _initRotation;
	FVector _initScale;

#ifdef UE_5_0_OR_LATER
	USkinnedAsset* GetSkinnedAsset() const { return _poseableMesh->GetSkinnedAsset(); }
#else
	USkeletalMesh* GetSkinnedAsset() const { return _poseableMesh->SkeletalMesh; }
#endif


	FTransform GetBoneLocalTransformByName(FName boneName) const
	{
		if (!GetSkinnedAsset() || !_poseableMesh->RequiredBones.IsValid())
		{
			LOG("Error: cannot get bone transform for: %s, mesh is invalid!", *boneName.ToString());
			return FTransform();
		}


		int32 boneIndex = _poseableMesh->GetBoneIndex(boneName);
		if (boneIndex >= 0 && boneIndex < _poseableMesh->BoneSpaceTransforms.Num())
		{
			return _poseableMesh->BoneSpaceTransforms[boneIndex];
		}

		return FTransform();
	}

	void SetBoneLocalTransformByName(FName boneName, const FTransform& InTransform)
	{
		if (!GetSkinnedAsset() || !_poseableMesh->RequiredBones.IsValid())
		{
			LOG("Error: cannot set bone transform for: %s, mesh is invalid!", *boneName.ToString());
			return;
		}

		int32 boneIndex = _poseableMesh->GetBoneIndex(BoneName);
		if (boneIndex >= 0 && boneIndex < _poseableMesh->BoneSpaceTransforms.Num())
		{
			_poseableMesh->BoneSpaceTransforms[boneIndex] = InTransform;
			_poseableMesh->MarkRefreshTransformDirty();
		}
	}

public:
	int BoneIndex{ -1 };
	FName BoneName{ "None" };

	const FVector& initLocalPosition() const { return _initLocalPosition; }
	const FQuat& initLocalRotation() const { return _initLocalRotation; }
	const FVector& initLocalScale() const { return _initLocalScale; }

	const FVector& initPosition() const { return _initPosition; }
	const FQuat& initRotation() const { return _initRotation; }
	const FVector& initScale() const { return _initScale; }

	FMatrix localToWorldMatrix() { return !_poseableMesh ? FMatrix::Identity : _poseableMesh->GetBoneMatrix(BoneIndex); }

	FVector position() const { return !_poseableMesh ? FVector() : _poseableMesh->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace); }
	FQuat rotation()  const { return !_poseableMesh ? FQuat::Identity : _poseableMesh->GetBoneRotationByName(BoneName, EBoneSpaces::WorldSpace).Quaternion(); }
	FVector lossyScale() const { return !_poseableMesh ? FVector() : _poseableMesh->GetBoneScaleByName(BoneName, EBoneSpaces::WorldSpace); }

	FVector localPosition() const { return GetBoneLocalTransformByName(BoneName).GetLocation(); }
	FQuat localRotation() const { return GetBoneLocalTransformByName(BoneName).GetRotation(); }
	FVector localScale() const { return GetBoneLocalTransformByName(BoneName).GetScale3D(); }


	void SetPosition(const FVector& value) { if (_poseableMesh) _poseableMesh->SetBoneLocationByName(BoneName, value, EBoneSpaces::WorldSpace); }
	void SetRotation(const FQuat& value) { if (_poseableMesh) _poseableMesh->SetBoneRotationByName(BoneName, value.Rotator(), EBoneSpaces::WorldSpace); }
	void SetLossyScale(const FVector& value) { if (_poseableMesh) _poseableMesh->SetBoneScaleByName(BoneName, value, EBoneSpaces::WorldSpace); }



	void SetLocalPosition(const FVector& value) {
		auto localToParent = GetBoneLocalTransformByName(BoneName);
		localToParent.SetLocation(value);
		SetBoneLocalTransformByName(BoneName, localToParent);
	}
	void SetLocalRotation(const FQuat& value) {
		auto localToParent = GetBoneLocalTransformByName(BoneName);
		localToParent.SetRotation(value.GetNormalized());
		SetBoneLocalTransformByName(BoneName, localToParent);
	}
	void SetLocalScale(const FVector& value) {
		auto localToParent = GetBoneLocalTransformByName(BoneName);
		localToParent.SetScale3D(value);
		SetBoneLocalTransformByName(BoneName, localToParent);
	}

	void Apply() {
		_poseableMesh->FinalizeBoneTransform();
	}

	BoneTransform(int boneIndex, const FName& boneName, UPoseableMeshComponent* poseableMesh) {
		BoneIndex = boneIndex;
		BoneName = boneName;
		_poseableMesh = poseableMesh;

		_initLocalPosition = localPosition();
		_initLocalRotation = localRotation();
		_initLocalScale = localScale();

		_initPosition = position();
		_initRotation = rotation();
		_initScale = lossyScale();
	}

	BoneTransform(const FName& boneName, UPoseableMeshComponent* poseableMesh) :
		BoneTransform(poseableMesh ? poseableMesh->GetBoneIndex(boneName) : -1, boneName, poseableMesh) {
	}


	BoneTransform() = default;

	const TSharedPtr<BoneTransform>& GetParnet() const { return _parent; }
	static void SetParent(const TSharedPtr<BoneTransform>& parent, const TSharedPtr<BoneTransform>& child) {
		child->_parent = parent; child->_parent->_children.Add(child);
	}

	const TArray<TSharedPtr<BoneTransform>>& GetChildren() const { return _children; }
	const TSharedPtr<BoneTransform>& GetChild(int index) const { return _children[index]; }
	TSharedPtr<BoneTransform> GetChildOrNull(int index) const { return _children.Num() > index ? _children[index] : nullptr; }
};