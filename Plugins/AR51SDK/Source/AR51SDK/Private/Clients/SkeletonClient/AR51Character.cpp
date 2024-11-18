#include "AR51Character.h"

#include <Engine\StaticMeshActor.h>
#include <DrawDebugHelpers.h>

#include "Converter.h"
#include "Utils.h"
#include "AR51UnrealSDK.h"
#include "Keypoints.h"
#include "BoneNames.h"
#include "BoneTransform.h"
#include <HandJointType.h>

class SkeletonData
{
private:
	FVector _;
public:
	TArray<FVector> Positions;

	FVector& operator [] (Keypoints index) { return Positions[(int)index]; }

	SkeletonData(const TArray<FVector>& positions) { Positions = positions; }

	FQuat GetRotation(Keypoints rBottom, Keypoints lBottom, Keypoints rTop, Keypoints lTop,
		FVector& forward)
	{
		return GetRotation((*this)[rBottom], (*this)[lBottom], 0.5f * ((*this)[rTop] + (*this)[lTop]), forward);
	}

	/// <summary>
	/// Helper functions that calculate rotation matrixx/quaterion from the position of triangle
	/// </summary>
	static FQuat GetRotation(FVector rightPoint, FVector leftPoint, FVector topPoint, FVector& forward)
	{
		auto right = (rightPoint - leftPoint).GetSafeNormal();
		auto centerPoint = 0.5f * (rightPoint + leftPoint);
		auto up = (topPoint - centerPoint).GetSafeNormal();
		forward = FVector::CrossProduct(right, up).GetSafeNormal();
		up = FVector::CrossProduct(forward, right).GetSafeNormal();
		forward = FVector::CrossProduct(right, up).GetSafeNormal();

		auto rotation = Utils::CreateRotation(right, up, forward);
		return rotation;
	}

	FQuat GetHipRotation() { return GetHipRotation(_); }
	FQuat GetHipRotation(FVector& forward) {
		return GetRotation(Keypoints::RHip, Keypoints::LHip, Keypoints::RShoulder, Keypoints::LShoulder, forward);
	}

	FQuat GetHeadRotation()
	{
		auto lEye = (*this)[Keypoints::LEye];
		auto rEye = (*this)[Keypoints::REye];
		auto lEar = (*this)[Keypoints::LEar];
		auto rEar = (*this)[Keypoints::REar];
		auto nose = (*this)[Keypoints::Nose];

		auto centerEye = 0.5f * (rEye + lEye);
		auto centerEar = 0.5f * (rEar + lEar);
		auto projectedEyes = Utils::ProjectOnVector(centerEar, nose, centerEye);

		auto headUp = (centerEye - projectedEyes).GetSafeNormal();
		auto headForward = (nose - centerEar).GetSafeNormal();
		auto headRight = -FVector::CrossProduct(headForward, headUp);

		return Utils::CreateRotation(headRight, headUp);
	}

	FQuat GetSpineRotation() { return GetSpineRotation(_); }
	FQuat GetSpineRotation(FVector& forward)
	{
		auto midShoulders = 0.5f * ((*this)[Keypoints::RShoulder] + (*this)[Keypoints::LShoulder]);
		auto midHips = 0.5f * ((*this)[Keypoints::RHip] + (*this)[Keypoints::LHip]);
		auto up = (midShoulders - midHips).GetSafeNormal();
		auto right = ((*this)[Keypoints::RHip] - (*this)[Keypoints::LHip]).GetSafeNormal();
		forward = FVector::CrossProduct(right, up).GetSafeNormal();
		right = FVector::CrossProduct(forward, up).GetSafeNormal();
		forward = FVector::CrossProduct(right, up).GetSafeNormal();

		return Utils::CreateRotation(right, up, forward);
	}

	FQuat GetUpperChestRotation() { return GetUpperChestRotation(_); };
	FQuat GetUpperChestRotation(FVector& forward)
	{
		auto midShoulders = 0.5f * ((*this)[Keypoints::RShoulder] + (*this)[Keypoints::LShoulder]);
		auto midHips = 0.5f * ((*this)[Keypoints::RHip] + (*this)[Keypoints::LHip]);
		auto up = (midShoulders - midHips).GetSafeNormal();

		return GetRotation((*this)[Keypoints::RShoulder], (*this)[Keypoints::LShoulder], midShoulders + up, forward);
	}

	FQuat GetLimbRotation(Keypoints topLimbKp, Keypoints midLimbKp, Keypoints bottomLimpKp, FVector& forward)
	{
		auto top = (*this)[topLimbKp];
		auto mid = (*this)[midLimbKp];
		auto bottom = (*this)[bottomLimpKp];

		auto right = (mid - top).GetSafeNormal();
		auto up = (bottom - mid).GetSafeNormal();
		auto f = FVector::CrossProduct(right, up).GetSafeNormal();
		up = FVector::CrossProduct(f, right).GetSafeNormal();

		auto rotation = Utils::CreateRotation(right, up);

		auto shoulderToWristDir = (bottom - top).GetSafeNormal();
		auto centerOnShoulderToWristPoint = FVector::DotProduct(mid - top, shoulderToWristDir) * shoulderToWristDir + top;
		forward = (centerOnShoulderToWristPoint - mid).GetSafeNormal();
		return rotation;
	}
	FQuat GetLShoulderRotation() { return GetLShoulderRotation(_); }
	FQuat GetLShoulderRotation(FVector& forward) {
		return GetLimbRotation(Keypoints::LShoulder, Keypoints::LElbow, Keypoints::LWrist, forward);
	}

	FQuat GetRShoulderRotation() { return GetRShoulderRotation(_); }
	FQuat GetRShoulderRotation(FVector& forward) {
		return
			GetLimbRotation(Keypoints::RShoulder, Keypoints::RElbow, Keypoints::RWrist, forward);
	}

	FQuat GetLHipRotation() { return GetLHipRotation(_); }
	FQuat GetLHipRotation(FVector& forward) {
		return
			GetLimbRotation(Keypoints::LHip, Keypoints::LKnee, Keypoints::LAnkle, forward);
	}

	FQuat GetRHipRotation() { return GetRHipRotation(_); }
	FQuat GetRHipRotation(FVector& forward) {
		return
			GetLimbRotation(Keypoints::RHip, Keypoints::RKnee, Keypoints::RAnkle, forward);
	}
};





class HandIKSolver
{
private:
	FQuat _prevWristRotationLeft;
	FQuat _prevWristRotationRight;

	bool _initialized{ false };
	float _originalIndexPinkyDistanceLeft;
	float _originalIndexPinkyDistanceRight;
	FVector _originalWristScaleLeft;
	FVector _originalWristScaleRight;
	FQuat _initWristRotationLeft;
	TArray<TSharedPtr<BoneTransform>> _jointsLeft;

	FQuat _initWristRotationRight;
	TArray<TSharedPtr<BoneTransform>> _jointsRight;

private:
	static FQuat GetWristRotation(FVector wrist, FVector index, FVector pinky)
	{
		auto right = (index - pinky).GetSafeNormal();
		auto forward = ((index + pinky) * 0.5f - wrist).GetSafeNormal();
		return Utils::CreateRotation(right, forward);
		//return FQuat::Identity;
	}

	const TSharedPtr<BoneTransform>& GetLeftJoint(EHandJointType joint) const { return _jointsLeft[(int)joint]; }
	const TSharedPtr<BoneTransform>& GetRightJoint(EHandJointType joint) const { return _jointsRight[(int)joint]; }

public:
	bool Initialize(const TArray<TSharedPtr<BoneTransform>>& leftJoints, const TArray<TSharedPtr<BoneTransform>>& rightJoints)
	{
		// Left
		_jointsLeft = leftJoints;
		
		auto leftWristRotation = GetWristRotation(
			GetLeftJoint(EHandJointType::Wrist)->position(),
			GetLeftJoint(EHandJointType::IndexProximal)->position(),
			GetLeftJoint(EHandJointType::LittleProximal)->position());

		_originalWristScaleLeft = GetLeftJoint(EHandJointType::Wrist)->localScale();
		_originalIndexPinkyDistanceLeft = (GetLeftJoint(EHandJointType::LittleProximal)->position() - GetLeftJoint(EHandJointType::IndexProximal)->position()).Size();
		_initWristRotationLeft = leftWristRotation.Inverse() * _jointsLeft[0]->rotation();
		_prevWristRotationLeft = _initWristRotationLeft;

		// Right
		_jointsRight = rightJoints;

		auto rightWristRotation = GetWristRotation(
			GetRightJoint(EHandJointType::Wrist)->position(),
			GetRightJoint(EHandJointType::IndexProximal)->position(),
			GetRightJoint(EHandJointType::LittleProximal)->position());

		_originalWristScaleRight = GetRightJoint(EHandJointType::Wrist)->localScale();
		_originalIndexPinkyDistanceRight = (GetRightJoint(EHandJointType::LittleProximal)->position() - GetRightJoint(EHandJointType::IndexProximal)->position()).Size();
		_initWristRotationRight = rightWristRotation.Inverse() * _jointsRight[0]->rotation();
		_prevWristRotationRight = _initWristRotationRight;

		_initialized = true;
		return _initialized;
	}

	static void LookAt(const FVector& src, const FVector& srcChild, const TSharedPtr<BoneTransform>& dst, const FQuat& initRotation)
	{
		if (!dst) return;

		dst->SetLocalRotation(initRotation);
		auto srcDir = (srcChild - src).GetSafeNormal();

		auto childPos = dst->GetChildren().Num() == 0 ?
			dst->position() + (dst->position() - dst->GetParnet()->position()) :
			dst->GetChild(0)->position();

		auto dstDir = (childPos - dst->position()).GetSafeNormal();

		if (FMath::Abs(FVector::DotProduct(srcDir, dstDir)) >= 1.0f) return;

		auto axis = FVector::CrossProduct(srcDir, dstDir).GetSafeNormal();
		auto degrees = Utils::SignedAngle(srcDir, dstDir, axis);
		auto radians = FMath::DegreesToRadians(degrees);

		dst->SetRotation(FQuat(axis, -radians) * dst->rotation());
	}

	static void AutoScale(const TArray<FVector>& src, const TArray<TSharedPtr<BoneTransform>>& dst, const FVector& originalScale, float originalDistance)
	{
		auto dstDistance = (dst[(int)EHandJointType::IndexProximal]->position() - dst[(int)EHandJointType::LittleProximal]->position()).Size();

		auto srcDistance = (src[(int)EHandJointType::IndexProximal] - src[(int)EHandJointType::LittleProximal]).Size();

		auto localScale = dst[0]->localScale() * (srcDistance / dstDistance);
		dst[0]->SetLocalScale(localScale);

		//if (SkeletonConsumer.Instance && SkeletonConsumer.Instance.HandAutoScaleMode == HandAutoScaleMode.Default)
		//{
		//	dst[0].localScale *= originalDistance / dstDistance;
		//}
		//else if (SkeletonConsumer.Instance && SkeletonConsumer.Instance.HandAutoScaleMode == HandAutoScaleMode.NonUniform)
		//{
		//	auto srcDistance = (src[(int)EHandJointType::IndexProximal] - src[(int)EHandJointType::LittleProximal]).magnitude;
		//	dst[0].localScale *= srcDistance / dstDistance;
		//}
		//else
		//{
		//	dst[0].localScale = originalScale;
		//}
	}
	
	/// <summary>
	/// Checks if there is at least one non-zero vector in the given list.
	/// </summary>
	/// <param name="jointPositions">List of joint positions to check.</param>
	/// <returns>True if there is at least one non-zero vector, false otherwise.</returns>
	bool IsValid(const TArray<FVector>& jointPositions) {

		for (const FVector& pos : jointPositions) {
			if (pos != FVector::ZeroVector)
				return true;
		}
		return false;
	}

	/// <summary>
	/// Solves the IK for one side of the body (either left or right), given the joint positions. 
	/// This method handles both valid and invalid joint positions, implementing a fallback to initial rotations if the joint positions are invalid.
	/// </summary>
	/// <param name="jointPositions">The array of joint positions.</param>
	/// <param name="joints">The array of joint bone transforms.</param>
	/// <param name="initWristRotation">The initial rotation of the wrist.</param>
	/// <param name="prevWristLocalRotation">The previous local rotation of the wrist.</param>
	/// <param name="copyWristPosition">Whether to copy the wrist position or not. Defaults to false.</param>
	void SolveOneSide(const TArray<FVector>& jointPositions, const TArray<TSharedPtr<BoneTransform>>& joints, const FQuat& initWristRotation, FQuat& prevWristLocalRotation, bool copyWristPosition = false)
	{
		auto wristJoint = joints[(int)EHandJointType::Wrist];

		// Check if the joint positions are valid
		if (IsValid(jointPositions))
		{
			auto originalWristScale = wristJoint->initLocalScale();
			auto originalIndexPinkyDistance = (joints[(int)EHandJointType::LittleProximal]->initPosition() - joints[(int)EHandJointType::IndexProximal]->initPosition()).Size();

			// Automatically scale the joints based on the joint positions
			AutoScale(jointPositions, joints, originalWristScale, originalIndexPinkyDistance);

			// If requested, copy the wrist position
			if (copyWristPosition) wristJoint->SetPosition(jointPositions[(int)EHandJointType::Wrist]);

			auto wristWorldRotation = GetWristRotation(
				jointPositions[(int)EHandJointType::Wrist],
				jointPositions[(int)EHandJointType::IndexProximal],
				jointPositions[(int)EHandJointType::LittleProximal]);

			// Set the wrist rotation
			wristJoint->SetRotation(wristWorldRotation * initWristRotation);

			// Iterate over each joint
			for (auto i = 0; i < joints.Num(); i++)
			{
				if (!joints[i] || joints[i]->BoneIndex == wristJoint->BoneIndex || HandJointTypeHelper::IsTip((EHandJointType)i)) continue;

				// Orient the joint towards the next joint in the hierarchy
				LookAt(jointPositions[i], jointPositions[i + 1], joints[i], joints[i]->initLocalRotation());
			}
		}
		else {
			// If joint positions are not valid, set the previous wrist rotation
			wristJoint->SetLocalRotation(prevWristLocalRotation);

			// Iterate over each joint
			for (size_t i = 0; i < joints.Num(); i++)
			{
				if (joints[i] == nullptr) continue;

				// Transition the joint's rotation and scale based on the hand tracking fallback smoothing factor
				joints[i]->SetLocalRotation(FMath::Lerp(joints[i]->initLocalRotation(), joints[i]->localRotation(), USkeletonConsumer::Instance()->HandTrackingFallbackSmoothingFactor));
				joints[i]->SetLocalScale(FMath::Lerp(joints[i]->initLocalScale(), joints[i]->localScale(), USkeletonConsumer::Instance()->HandTrackingFallbackSmoothingFactor));
			}
		}
	}

public:

	const TArray<TSharedPtr<BoneTransform>>& GetLeftHandJoints() const { return _jointsLeft; }
	const TArray<TSharedPtr<BoneTransform>>& GetRightHandJoints() const { return _jointsRight; }

	/// <summary>
	/// Solves two hand ik based on specified transform.
	/// The specified transform must contain an array of transforms that matches the enum <see cref="HandJointType"/>
	/// </summary>
	void Solve(const TArray<FVector>& jointsLeft, const TArray<FVector>& jointsRight, bool copyWristPosition = false)
	{
		if (!_initialized) return;

		// left & right fingers
		SolveOneSide(jointsLeft, _jointsLeft, _initWristRotationLeft, _prevWristRotationLeft, copyWristPosition);
		SolveOneSide(jointsRight, _jointsRight, _initWristRotationRight, _prevWristRotationRight, copyWristPosition);

		_prevWristRotationLeft = _jointsLeft[0]->localRotation();
		_prevWristRotationRight = _jointsRight[0]->localRotation();
	}
};

class JointInfo
{
public:
	TSharedPtr<BoneTransform> transform{};
	Keypoints Keypoint{ Keypoints::Unknown };
	FMatrix BindPose{ FMatrix::Identity };

	FVector Position{ FVector::ZeroVector };
	FQuat Rotation{ FQuat::Identity };
	FVector LossyScale{ FVector::OneVector };

	FVector LocalPosition{ FVector::ZeroVector };
	FQuat LocalRotation{ FQuat::Identity };
	FVector LocalScale{ FVector::OneVector };

	FQuat BindPoseInverseRotation{ FQuat::Identity };

	JointInfo() = default;
	JointInfo(TSharedPtr<BoneTransform> joint, Keypoints keypoint)
	{
		transform = joint;
		Keypoint = keypoint;

		BindPose = joint->localToWorldMatrix();

		Position = joint->position();
		Rotation = joint->rotation();
		LossyScale = joint->lossyScale();

		LocalPosition = joint->localPosition();
		LocalRotation = joint->localRotation();
		LocalScale = joint->localScale();

		BindPoseInverseRotation = FQuat::Identity;
	}
};

class SkeletonInfo
{
private:
	FTransform _hipsParentTransform;
	FTransform _hipsTransform;
	FSolverParams* _parameters;
public:

	TArray<JointInfo> JointInfos{ };

	JointInfo& operator [] (Keypoints index) { return JointInfos[(int)index]; }

	SkeletonInfo(FSolverParams* parameters, const TArray<TSharedPtr<BoneTransform>>& ts, const TArray<FMatrix>& bindPose, const TArray<int>& hierarchy, const TArray<int> keypoints)
	{
		_parameters = parameters;
		JointInfos.AddDefaulted(KeypointsHelper::AllKeypoints().Num());

		for (auto kp : KeypointsHelper::AllKeypoints())
		{
			auto kpIndex = (int)kp;
			if (kpIndex >= keypoints.Num() || kpIndex < 0 || keypoints[kpIndex] < 0) continue;

			auto transformIndex = keypoints[kpIndex];
			//auto ti = TransformInfos[transformIndex];
			auto t = ts[transformIndex];
			JointInfos[kpIndex] = JointInfo(t, kp);
		}
	}

private:
	void LookAt(JointInfo& parent, JointInfo& child, const FVector& parentPosition, const FVector& childPosition, bool resetRotation = true)
	{
		if (resetRotation) parent.transform->SetLocalRotation(parent.LocalRotation);

		auto srcAim = (child.transform->position() - parentPosition).GetSafeNormal();
		auto dstAim = (childPosition - parentPosition).GetSafeNormal();

		if (FVector::DotProduct(srcAim, dstAim) >= .999f) return;

		auto axis = FVector::CrossProduct(dstAim, srcAim).GetSafeNormal();
		auto degrees = Utils::SignedAngle(srcAim, dstAim, axis);
		auto radians = FMath::DegreesToRadians(degrees);
		parent.transform->SetRotation(FQuat(axis, radians) * parent.transform->rotation());
	}

	void LookAt(JointInfo& parent, JointInfo& child, const FVector& childPosition, bool resetRotation = true) {
		LookAt(parent, child, parent.transform->position(), childPosition, resetRotation);
	}

public:
	bool IsEnabled() const { return _parameters->Enabled; }
	void Initialize(SkeletonData skeleton)
	{
		auto headPosition = (*this)[Keypoints::Neck].transform->position();
		auto hipPosition = 0.5f * (skeleton[Keypoints::RHip] + skeleton[Keypoints::LHip]);
		auto bodyRight = skeleton[Keypoints::RHip] - skeleton[Keypoints::LHip];
		auto bodyUp = headPosition - hipPosition;
		auto bodyRotation = Utils::CreateRotation(bodyRight.GetSafeNormal(), bodyUp.GetSafeNormal());

		// init hip bind pose
		auto& hipJoin = (*this)[Keypoints::MidHip];
		hipJoin.BindPoseInverseRotation = skeleton.GetHipRotation().Inverse();

		_hipsParentTransform = FTransform::Identity;
		_hipsParentTransform.SetTranslation(hipPosition);
		_hipsParentTransform.SetScale3D(hipJoin.transform->localScale());

		_hipsTransform = FTransform::Identity;
		_hipsTransform.SetTranslation(hipJoin.transform->position());
		_hipsTransform.SetRotation(hipJoin.transform->rotation());

		_hipsTransform = _hipsTransform.GetRelativeTransform(_hipsParentTransform);

		// init spine bind pose
		auto& spineJoint = (*this)[Keypoints::Spine];
		spineJoint.BindPoseInverseRotation = skeleton.GetSpineRotation().Inverse() * spineJoint.Rotation;

		// init chest bind pose
		auto& chestJoint = (*this)[Keypoints::UpperChest];
		chestJoint.BindPoseInverseRotation = skeleton.GetUpperChestRotation().Inverse() * chestJoint.Rotation;

		// init neck joint
		auto& neckJoint = (*this)[Keypoints::Neck];
		neckJoint.BindPoseInverseRotation = bodyRotation.Inverse() * neckJoint.Rotation;

		// init arm joint
		auto& lShoulderJoint = (*this)[Keypoints::LShoulder];
		lShoulderJoint.BindPoseInverseRotation = skeleton.GetLShoulderRotation().Inverse() * lShoulderJoint.Rotation;
		auto& rShoulderJoint = (*this)[Keypoints::RShoulder];
		rShoulderJoint.BindPoseInverseRotation = skeleton.GetRShoulderRotation().Inverse() * rShoulderJoint.Rotation;

		// init LShoulder joint
		auto& lHipJoint = (*this)[Keypoints::LHip];
		lHipJoint.BindPoseInverseRotation = skeleton.GetLHipRotation().Inverse() * lHipJoint.Rotation;
		auto& rHipJoint = (*this)[Keypoints::RHip];
		rHipJoint.BindPoseInverseRotation = skeleton.GetRHipRotation().Inverse() * rHipJoint.Rotation;
	}

	void ApplyNoneUniformScale(SkeletonData& skeleton) {
		auto& hips = (*this)[Keypoints::MidHip];

		// scale hips
		auto lHipsScale = (skeleton[Keypoints::LShoulder] - skeleton[Keypoints::MidHip]).Size() /
			((*this)[Keypoints::LShoulder].Position - (*this)[Keypoints::MidHip].Position).Size();
		auto rHipsScale = (skeleton[Keypoints::RShoulder] - skeleton[Keypoints::MidHip]).Size() /
			((*this)[Keypoints::RShoulder].Position - (*this)[Keypoints::MidHip].Position).Size();
		auto hipsScale = 0.5f * (rHipsScale + lHipsScale);

		auto oldHipsScale = hips.transform->localScale().X;
		auto newHipsScale = (hips.LocalScale.X * hipsScale);

		if (!USkeletonConsumer::Instance() || USkeletonConsumer::Instance()->SolverParamters.IsValidScale(newHipsScale)) {
			auto filteredHipsScale = ApplyAutoScaleFilter(oldHipsScale, newHipsScale);
			hips.transform->SetLocalScale(filteredHipsScale * FVector::OneVector);
		}

		// scale left
		ScaleJoint(skeleton, Keypoints::LShoulder, Keypoints::LElbow);
		ScaleJoint(skeleton, Keypoints::LElbow, Keypoints::LWrist);
		ScaleJoint(skeleton, Keypoints::LHip, Keypoints::LKnee);
		ScaleJoint(skeleton, Keypoints::LKnee, Keypoints::LAnkle);

		// scale right
		ScaleJoint(skeleton, Keypoints::RShoulder, Keypoints::RElbow);
		ScaleJoint(skeleton, Keypoints::RElbow, Keypoints::RWrist);
		ScaleJoint(skeleton, Keypoints::RHip, Keypoints::RKnee);
		ScaleJoint(skeleton, Keypoints::RKnee, Keypoints::RAnkle);
	}

	void ResetScale() {
		for (auto& ji : JointInfos) {
			ji.transform->SetLocalScale(ji.LocalScale);
		}
	}
	FQuat GetWristRotation(const FVector& wrist, const FVector& index, const FVector& pinky)
	{
		auto right = (index - pinky).GetSafeNormal();
		auto forward = ((index + pinky) * 0.5f - wrist).GetSafeNormal();
		
		return Utils::CreateRotation(right, forward);
	}



	void Solve(SkeletonData& skeleton)
	{
		ApplyNoneUniformScale(skeleton);

		// Solve Hips Rotation & Postiion
		auto& hipJoint = (*this)[Keypoints::MidHip];
		FVector hipsForward;
		_hipsParentTransform.SetRotation(skeleton.GetHipRotation(hipsForward) * hipJoint.BindPoseInverseRotation);
		_hipsParentTransform.SetTranslation(skeleton[Keypoints::MidHip]);
		_hipsParentTransform.SetScale3D(hipJoint.transform->localScale());

		auto t = _hipsTransform * _hipsParentTransform;
		hipJoint.transform->SetPosition(t.GetTranslation());
		hipJoint.transform->SetRotation(t.GetRotation());

		// Solve spine
		auto& spineJoint = (*this)[Keypoints::Spine];
		spineJoint.transform->SetRotation(skeleton.GetSpineRotation() * spineJoint.BindPoseInverseRotation);

		// Solve Chest
		auto& chestJoint = (*this)[Keypoints::UpperChest];
		chestJoint.transform->SetRotation(skeleton.GetUpperChestRotation() * chestJoint.BindPoseInverseRotation);

		//// Solve Head
		auto& neckJoint = (*this)[Keypoints::Neck];
		neckJoint.transform->SetRotation(skeleton.GetHeadRotation() * neckJoint.BindPoseInverseRotation);

		// left Arms
		SolveArm(skeleton, hipsForward, Keypoints::LShoulder, Keypoints::LElbow, Keypoints::LWrist);
		SolveArm(skeleton, hipsForward, Keypoints::RShoulder, Keypoints::RElbow, Keypoints::RWrist);

		// Solve Legs
		SolveLeg(skeleton, hipsForward, Keypoints::LHip, Keypoints::LKnee, Keypoints::LAnkle, Keypoints::LBigToe);
		SolveLeg(skeleton, hipsForward, Keypoints::RHip, Keypoints::RKnee, Keypoints::RAnkle, Keypoints::RBigToe);
	}

	//FVector GetGroundedAnkle(SkeletonData& skeleton, bool isRight) {
	//	auto knee = isRight ? Keypoints::RKnee : Keypoints::LKnee;
	//	auto ankle = isRight ? Keypoints::RAnkle : Keypoints::LAnkle;
	//	auto toe = isRight ? Keypoints::RBigToe : Keypoints::LBigToe;
	//	auto kneeToAnkle = skeleton[ankle] - skeleton[knee];
	//	auto offset = GetFeetOffset(knee, ankle, toe);
	//	auto ankleOffsetedPosition = skeleton[ankle] + offset * kneeToAnkle.GetSafeNormal();
	//	return ankleOffsetedPosition;
	//}

private:

	double ApplyAutoScaleFilter(double oldValue, double newValue) {
		double scale = 0;
		switch (_parameters->FilterType)
		{
		case EAutoScaleFilterType::FT_SimpleSmoothing:
			scale = _parameters->SmoothingFactor * oldValue + (1 - _parameters->SmoothingFactor) * newValue;
			break;
		case EAutoScaleFilterType::FT_Default:
		default:
			scale = newValue;
			break;
		}
		return scale;
	}

	void ScaleJoint(SkeletonData& skeleton, Keypoints parent, Keypoints child) {
		auto& baseJoint = (*this)[parent];
		auto& midJoint = (*this)[child];

		auto baseScale = (skeleton[parent] - skeleton[child]).Size() /
			(baseJoint.transform->position() - midJoint.transform->position()).Size();

		auto oldValue = baseJoint.transform->localScale().X;
		auto newValue = baseScale * oldValue;

		if (!USkeletonConsumer::Instance() || USkeletonConsumer::Instance()->SolverParamters.IsValidScale(newValue)) {
			auto filteredScale = ApplyAutoScaleFilter(oldValue, newValue);
			baseJoint.transform->SetLocalScale(filteredScale * FVector::OneVector);
		}
	}

	void SolveArm(SkeletonData& skeleton, FVector hipsForward, Keypoints shoulder, Keypoints elbow, Keypoints wrist)
	{
		auto& joint = (*this)[shoulder];

		// set shoulder position
		if(_parameters->EnableLimbPositioning) joint.transform->SetPosition(skeleton[shoulder]);

		// get triangle rotation
		FVector forward;
		auto rotation = skeleton.GetLimbRotation(shoulder, elbow, wrist, forward);

		if (FVector::DotProduct(hipsForward.GetSafeNormal(), forward.GetSafeNormal()) < -.5f)
			LookAt((*this)[shoulder], (*this)[elbow], skeleton[elbow], false);
		else
			joint.transform->SetRotation(rotation * joint.BindPoseInverseRotation);

		LookAt((*this)[elbow], (*this)[wrist], skeleton[wrist]);
	}

	void SolveLeg(SkeletonData& skeleton, FVector hipsForward, Keypoints hip, Keypoints knee, Keypoints ankle, Keypoints toe)
	{
		auto& hipJoint = (*this)[hip];

		// set hip position
		if(_parameters->EnableLimbPositioning)  hipJoint.transform->SetPosition(skeleton[hip]);

		// get triangle rotation
		FVector forward;
		auto rotation = skeleton.GetLimbRotation(hip, knee, ankle, forward);

		auto hipToAnkle = (skeleton[ankle] - skeleton[hip]).GetSafeNormal();
		auto knee2Ankle = (skeleton[ankle] - skeleton[knee]).GetSafeNormal();

		// rotate hip
		// don't use triangle rotation if the triangle's forward does not match body forward,
		// or when the leg is too streight, i.e. the angle between the knee 2 ankle and hip 2 ankle is low (dot close to 1)
		if (FVector::DotProduct(hipsForward.GetSafeNormal(), -forward.GetSafeNormal()) < .41f || FVector::DotProduct(hipToAnkle, knee2Ankle) > 0.975f)
			LookAt((*this)[hip], (*this)[knee], skeleton[knee], false);
		else
			hipJoint.transform->SetRotation(rotation * hipJoint.BindPoseInverseRotation);

		// rotate knee
		LookAt((*this)[knee], (*this)[ankle], skeleton[ankle]);

		// rotate ankle/foot
		LookAt((*this)[ankle], (*this)[toe], skeleton[ankle], skeleton[toe]);
	}

};

// Sets default values for this component's properties
UAR51Character::UAR51Character()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

FName UAR51Character::GetBoneName(FName& name) {
	auto p = _nodeMapping.Find(name);
	return p ? *p : name;
}

// Called when the game starts
void UAR51Character::BeginPlay()
{
	Super::BeginPlay();

	if (!USkeletonConsumer::Instance()) {
		LOG("Error: SkeletonConsumer instance could not be found, AR51Character cannot start.");
		return;
	}

	if (!SkeletonMapping) {
		LOG("Error: SkeletonMapping is null, AR51Character cannot start.");
		return;
	}

	SkeletonMapping->GetTargetToSourceMappingTable(_nodeMapping);

	BoneNames _boneNames;
	_boneNames.Pelvis = GetBoneName(_boneNames.Pelvis);
	_boneNames.spine_03 = GetBoneName(_boneNames.spine_03);
	_boneNames.neck_01 = GetBoneName(_boneNames.neck_01);
	_boneNames.head = GetBoneName(_boneNames.head);

	// Left Arm
	_boneNames.UpperArm_L = GetBoneName(_boneNames.UpperArm_L);
	_boneNames.lowerarm_l = GetBoneName(_boneNames.lowerarm_l);
	_boneNames.Hand_L = GetBoneName(_boneNames.Hand_L);

	// Left Leg
	_boneNames.Thigh_L = GetBoneName(_boneNames.Thigh_L);
	_boneNames.calf_l = GetBoneName(_boneNames.calf_l);
	_boneNames.Foot_L = GetBoneName(_boneNames.Foot_L);
	_boneNames.ball_l = GetBoneName(_boneNames.ball_l);

	// Right Arm
	_boneNames.UpperArm_R = GetBoneName(_boneNames.UpperArm_R);
	_boneNames.lowerarm_r = GetBoneName(_boneNames.lowerarm_r);
	_boneNames.Hand_R = GetBoneName(_boneNames.Hand_R);

	// Right Leg
	_boneNames.Thigh_R = GetBoneName(_boneNames.Thigh_R);
	_boneNames.calf_r = GetBoneName(_boneNames.calf_r);
	_boneNames.Foot_R = GetBoneName(_boneNames.Foot_R);
	_boneNames.ball_r = GetBoneName(_boneNames.ball_r);


	TArray<FName> boneNames;
	GetBoneNames(boneNames);
	TArray<int> hierarchy;
	TArray<int> keypoints;
	TArray<FVector> positions;
	TArray<FMatrix> bindposes;
	TArray<TSharedPtr<BoneTransform>> ts;

	keypoints.AddDefaulted(KeypointsHelper::AllKeypoints().Num());
	positions.AddDefaulted(KeypointsHelper::AllKeypoints().Num());
	for (int i = 0; i < keypoints.Num(); i++) keypoints[i] = -1;

	for (size_t i = 0; i < boneNames.Num(); i++)
	{
		auto parent = GetParentBone(boneNames[i]);
		auto parentIndex = boneNames.Find(parent);
		ts.Add(TSharedPtr<BoneTransform>(new BoneTransform(i, boneNames[i], this)));
		hierarchy.Add(parentIndex);
		bindposes.Add(GetBoneMatrix(i));
		if (parentIndex >= 0) {
			BoneTransform::SetParent(ts[parentIndex], ts.Last());
		}
	}

	TMap<Keypoints, FName> toHumanBodyBones = {
		{Keypoints::MidHip, _boneNames.Pelvis},
		{Keypoints::Spine, _boneNames.spine_01},
		{Keypoints::UpperChest, _boneNames.spine_03},
		{Keypoints::Neck, _boneNames.head},

		{Keypoints::LShoulder, _boneNames.UpperArm_L},
		{Keypoints::LElbow, _boneNames.lowerarm_l},
		{Keypoints::LWrist, _boneNames.Hand_L},

		{Keypoints::RShoulder, _boneNames.UpperArm_R},
		{Keypoints::RElbow, _boneNames.lowerarm_r},
		{Keypoints::RWrist, _boneNames.Hand_R},

		{Keypoints::LHip, _boneNames.Thigh_L},
		{Keypoints::LKnee, _boneNames.calf_l},
		{Keypoints::LAnkle, _boneNames.Foot_L},
		{Keypoints::LBigToe, _boneNames.ball_l},

		{Keypoints::RHip, _boneNames.Thigh_R},
		{Keypoints::RKnee, _boneNames.calf_r},
		{Keypoints::RAnkle, _boneNames.Foot_R},
		{Keypoints::RBigToe, _boneNames.ball_r},

		// left fingers
		{Keypoints::LThumbFinger1, _boneNames.thumb_01_l},
		{Keypoints::LThumbFinger2, _boneNames.thumb_02_l},
		{Keypoints::LThumbFinger3, _boneNames.thumb_03_l},

		{Keypoints::LIndexFinger1, _boneNames.index_01_l},
		{Keypoints::LIndexFinger2, _boneNames.index_02_l},
		{Keypoints::LIndexFinger3, _boneNames.index_03_l},

		{Keypoints::LMiddleFinger1, _boneNames.middle_01_l},
		{Keypoints::LMiddleFinger2, _boneNames.middle_02_l},
		{Keypoints::LMiddleFinger3, _boneNames.middle_03_l},

		{Keypoints::LRingFinger1, _boneNames.ring_01_l},
		{Keypoints::LRingFinger2, _boneNames.ring_02_l},
		{Keypoints::LRingFinger3, _boneNames.ring_03_l},

		{Keypoints::LPinkyFinger1, _boneNames.pinky_01_l},
		{Keypoints::LPinkyFinger2, _boneNames.pinky_02_l},
		{Keypoints::LPinkyFinger3, _boneNames.pinky_03_l},


		// right fingers
		{Keypoints::RThumbFinger1, _boneNames.thumb_01_r},
		{Keypoints::RThumbFinger2, _boneNames.thumb_02_r},
		{Keypoints::RThumbFinger3, _boneNames.thumb_03_r},

		{Keypoints::RIndexFinger1, _boneNames.index_01_r},
		{Keypoints::RIndexFinger2, _boneNames.index_02_r},
		{Keypoints::RIndexFinger3, _boneNames.index_03_r},

		{Keypoints::RMiddleFinger1, _boneNames.middle_01_r},
		{Keypoints::RMiddleFinger2, _boneNames.middle_02_r},
		{Keypoints::RMiddleFinger3, _boneNames.middle_03_r},

		{Keypoints::RRingFinger1, _boneNames.ring_01_r},
		{Keypoints::RRingFinger2, _boneNames.ring_02_r},
		{Keypoints::RRingFinger3, _boneNames.ring_03_r},

		{Keypoints::RPinkyFinger1, _boneNames.pinky_01_r},
		{Keypoints::RPinkyFinger2, _boneNames.pinky_02_r},
		{Keypoints::RPinkyFinger3, _boneNames.pinky_03_r},
	};

	if (bindposes.Num() == 0) // bindposes.IsEmpty() is not compatiable with unreal 4.27
	{
		LOG("Warning: bindposes is empty.");
		return;
	}

	for(auto kp : KeypointsHelper::AllKeypoints())
	{
		if (!toHumanBodyBones.Contains(kp)) continue;

		auto boneName = toHumanBodyBones[kp];
		keypoints[(int)kp] = boneNames.Find(boneName);
		if (keypoints[(int)kp] < bindposes.Num())
			positions[(int)kp] = bindposes[keypoints[(int)kp]].TransformPosition(FVector::ZeroVector);
		else
		{
			LOG("Error: bindposes is %d items long, and keypoints index is out of bounds at %d.", bindposes.Num(), keypoints[(int)kp]);
			return;
		}
	}

	_skeletonInfo = TSharedPtr<SkeletonInfo>(new SkeletonInfo(&USkeletonConsumer::Instance()->SolverParamters,
		ts, bindposes, hierarchy, keypoints));
	_skeletonInfo->Initialize(SkeletonData(positions));

	auto GetHandTransform = [&](Keypoints k) { return ts[keypoints[(int)k]]; };
	_leftHandJoints = {
		GetHandTransform(Keypoints::LWrist),

		// thumb
		GetHandTransform(Keypoints::LThumbFinger1)->GetParnet(),
		GetHandTransform(Keypoints::LThumbFinger1),
		GetHandTransform(Keypoints::LThumbFinger2),
		GetHandTransform(Keypoints::LThumbFinger3),
		GetHandTransform(Keypoints::LThumbFinger3)->GetChildOrNull(0),
		// Index
		GetHandTransform(Keypoints::LIndexFinger1),
		GetHandTransform(Keypoints::LIndexFinger2),
		GetHandTransform(Keypoints::LIndexFinger3),
		GetHandTransform(Keypoints::LIndexFinger3)->GetChildOrNull(0),
		// Middle
		GetHandTransform(Keypoints::LMiddleFinger1),
		GetHandTransform(Keypoints::LMiddleFinger2),
		GetHandTransform(Keypoints::LMiddleFinger3),
		GetHandTransform(Keypoints::LMiddleFinger3)->GetChildOrNull(0),
		// Ring
		GetHandTransform(Keypoints::LRingFinger1),
		GetHandTransform(Keypoints::LRingFinger2),
		GetHandTransform(Keypoints::LRingFinger3),
		GetHandTransform(Keypoints::LRingFinger3)->GetChildOrNull(0),
		// Pinky
		GetHandTransform(Keypoints::LPinkyFinger1)->GetParnet(),
		GetHandTransform(Keypoints::LPinkyFinger1),
		GetHandTransform(Keypoints::LPinkyFinger2),
		GetHandTransform(Keypoints::LPinkyFinger3),
		GetHandTransform(Keypoints::LPinkyFinger3)->GetChildOrNull(0),
	};
	_rightHandJoints = {
		GetHandTransform(Keypoints::RWrist),

		// thumb
		GetHandTransform(Keypoints::RThumbFinger1)->GetParnet(),
		GetHandTransform(Keypoints::RThumbFinger1),
		GetHandTransform(Keypoints::RThumbFinger2),
		GetHandTransform(Keypoints::RThumbFinger3),
		GetHandTransform(Keypoints::RThumbFinger3)->GetChildOrNull(0),
		// Index
		GetHandTransform(Keypoints::RIndexFinger1),
		GetHandTransform(Keypoints::RIndexFinger2),
		GetHandTransform(Keypoints::RIndexFinger3),
		GetHandTransform(Keypoints::RIndexFinger3)->GetChildOrNull(0),
		// Middle
		GetHandTransform(Keypoints::RMiddleFinger1),
		GetHandTransform(Keypoints::RMiddleFinger2),
		GetHandTransform(Keypoints::RMiddleFinger3),
		GetHandTransform(Keypoints::RMiddleFinger3)->GetChildOrNull(0),
		// Ring
		GetHandTransform(Keypoints::RRingFinger1),
		GetHandTransform(Keypoints::RRingFinger2),
		GetHandTransform(Keypoints::RRingFinger3),
		GetHandTransform(Keypoints::RRingFinger3)->GetChildOrNull(0),
		// Pinky
		GetHandTransform(Keypoints::RPinkyFinger1)->GetParnet(),
		GetHandTransform(Keypoints::RPinkyFinger1),
		GetHandTransform(Keypoints::RPinkyFinger2),
		GetHandTransform(Keypoints::RPinkyFinger3),
		GetHandTransform(Keypoints::RPinkyFinger3)->GetChildOrNull(0),
	};
	_handIKSolver = TSharedPtr<HandIKSolver>(new HandIKSolver());
	_handIKSolver->Initialize(_leftHandJoints, _rightHandJoints);
}

void UAR51Character::Solve(const TArray<FVector>& positions) {
	if (!_skeletonInfo) {
		LOG("Error: Solver canceled, Skeleton info is null.");
		return;
	}
	
	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Solver canceled, Skeleton info is disabled.");
		return;
	}

	SkeletonData skeletonData(positions);
	_skeletonInfo->Solve(skeletonData);
}

void UAR51Character::SolveHands(const TArray<FVector>& lHandPos, const TArray<FVector>& rHandPos) {
	if (!_handIKSolver) {
		LOG("Error: Hand solver canceled, Hand solver info is null.");
		return;
	}
	_handIKSolver->Solve(lHandPos, rHandPos);
}


FVector UAR51Character::GetHeadPosition() {
	if (!_skeletonInfo) {
		LOG("Error: Skeleton info is null.");
		return FVector::ZeroVector;
	}

	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Skeleton info is disabled.");
		return FVector::ZeroVector;
	}

	auto& neckJoint = (*_skeletonInfo)[Keypoints::Neck];
	return neckJoint.transform->position();
}

FQuat UAR51Character::GetHeadRotation() {

	if (!_skeletonInfo) {
		LOG("Error: Skeleton info is null.");
		return FQuat::Identity;
	}

	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Skeleton info is disabled.");
		return FQuat::Identity;
	}

	auto& neckJoint = (*_skeletonInfo)[Keypoints::Neck];
	return neckJoint.transform->rotation();
}

FVector UAR51Character::GetLeftWristPosition() {
	if (!_skeletonInfo) {
		LOG("Error: Skeleton info is null.");
		return FVector::ZeroVector;
	}

	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Skeleton info is disabled.");
		return FVector::ZeroVector;
	}

	auto& joint = (*_skeletonInfo)[Keypoints::LWrist];
	return joint.transform->position();
}

FVector UAR51Character::GetRightWristPosition() {
	if (!_skeletonInfo) {
		LOG("Error: Skeleton info is null.");
		return FVector::ZeroVector;
	}

	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Skeleton info is disabled.");
		return FVector::ZeroVector;
	}


	auto& joint = (*_skeletonInfo)[Keypoints::RWrist];
	return joint.transform->position();
}


const TArray<TSharedPtr<BoneTransform>>& UAR51Character::GetLeftHandJoints() const 
{ 
	if (!_handIKSolver)
		return _emptyJoints;
	return _handIKSolver->GetLeftHandJoints();
}

const TArray<TSharedPtr<BoneTransform>>& UAR51Character::GetRightHandJoints() const 
{ 
	if (!_handIKSolver)
		return _emptyJoints;
	return _handIKSolver->GetRightHandJoints(); 
}

void UAR51Character::SetHeadRotation(const FQuat& rotation) {
	if (!_skeletonInfo) {
		LOG("Error: Skeleton info is null.");
		return;
	}

	if (!_skeletonInfo->IsEnabled()) {
		LOG("Warning: Skeleton info is disabled.");
		return;
	}


	auto& neckJoint = (*_skeletonInfo)[Keypoints::Neck];
	neckJoint.transform->SetRotation(rotation * neckJoint.Rotation);
}


// Called every frame
void UAR51Character::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAR51Character::SetBoneLocalTransformByName(FName BoneName, const FTransform& InTransform)
{
	if (!GetSkinnedAsset() || !RequiredBones.IsValid())
		return;

	int32 boneIndex = GetBoneIndex(BoneName);
	if (boneIndex >= 0 && boneIndex < BoneSpaceTransforms.Num())
	{
		BoneSpaceTransforms[boneIndex] = InTransform;
		MarkRefreshTransformDirty();
	}
}


FTransform UAR51Character::GetBoneLocalTransformByName(FName BoneName)
{
	if (!GetSkinnedAsset() || !RequiredBones.IsValid())
	{
		LOG("Error: Skinned asset is invalid, defaulting to identity transform.");
		return FTransform();
	}

	int32 boneIndex = GetBoneIndex(BoneName);
	if (boneIndex >= 0 && boneIndex < BoneSpaceTransforms.Num())
	{
		return BoneSpaceTransforms[boneIndex];
	}

	LOG("Warning: Bone name: '%s' was not found, defaulting to identity transform.", *BoneName.ToString());
	return FTransform();
}