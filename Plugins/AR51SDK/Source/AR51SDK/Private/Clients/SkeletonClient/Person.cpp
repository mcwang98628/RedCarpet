#include "Person.h"

#include "UnityAdapter.h"
#include "Utils.h"
#include <AnchorServiceComponent.h>

const FString& Person::GetId() const { return _id; }

UAR51Character* Person::GetCharacter() const { return _character; }

AActor* Person::GetModel() const { return _character && _character->IsValidLowLevel() ? _character->GetOwner() : nullptr; }

const AR51::SkeletonReply& Person::GetLastSkeleton() const { return _lastSkeleton; }

/// <summary>
/// Sets the last skeleton data received from AR51 and updates the raw positions with respect to Unreal's world space.
/// </summary>
/// <param name="skeleton">The skeleton reply data from AR51.</param>
/// <param name="timestamp">The timestamp associated with the data.</param>
void Person::SetLastSkeleton(const AR51::SkeletonReply& skeleton, float timestamp) {
	// Store the received skeleton
	_lastSkeleton = skeleton;

	// Convert the skeleton positions to Unreal's FVector format
	_rawPositions = Converter::ToTArray<FVector>(skeleton.positions());

	// Convert the positions from Unity's space to Unreal's space (using unreal unit scale)
	UnityAdapter::ToUnrealInplace(_rawPositions, UnityAdapter::TO_UNREAL_UNIT_SCALE);

	// Convert the positions from Anchor space to World space
	UAnchorServiceComponent::AnchorToWorldSpace(_rawPositions);

	// Update the last update time
	_lastUpdateTime = timestamp;
}

void Person::FixCalfRatio(float calfRatio) {
	auto calf = _rawPositions[(int)Keypoints::LAnkle] - _rawPositions[(int)Keypoints::LKnee];
	_rawPositions[(int)Keypoints::LAnkle] = _rawPositions[(int)Keypoints::LKnee] + calfRatio * calf;

	calf = _rawPositions[(int)Keypoints::RAnkle] - _rawPositions[(int)Keypoints::RKnee];
	_rawPositions[(int)Keypoints::RAnkle] = _rawPositions[(int)Keypoints::RKnee] + calfRatio * calf;
}

const TArray<FVector>& Person::GetRawPositions() const { return _rawPositions; }

float Person::GetLastUpdateTime() const { return _lastUpdateTime; }

float Person::GetTimeFromLastUpdate(float currentTime) const { return currentTime - _lastUpdateTime; }

Person::Person(const FString& id, UAR51Character* character)
{
	_id = id;
	_character = character;
}

Person::~Person()
{
}
