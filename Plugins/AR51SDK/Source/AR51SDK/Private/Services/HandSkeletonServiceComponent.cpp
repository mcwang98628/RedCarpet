#include "Services/HandSkeletonServiceComponent.h"
#include <HandsAdapterFactory.h>
#include <HandJointInfo.h>
#include <UnrealThread.h>
#include <UnityAdapter.h>
#include <AnchorServiceComponent.h>

using namespace std;
using namespace AR51;
using namespace grpc;

UHandSkeletonServiceComponent::UHandSkeletonServiceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UHandSkeletonServiceComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UHandSkeletonServiceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
Status UHandSkeletonServiceComponent::StartStreaming(ServerContext* context, const Empty* request, ServerWriter<TwoHandsInfoResponse>* writer) {

	auto status = Status::OK;
	TwoHandsInfoResponse response;
	auto adapter = HandsAdapterFactory::GetAdapter();
	TArray<HandJointInfo> lHand, rHand;

	if (!adapter) {
		LOG("Error: Could not find an adapter.");
		return Status(StatusCode::ABORTED, "Error: Could not find an adapter.");
	}

	do {
		// get right & left hand positions and rotations on the main unreal thread
		auto isHandsTracked = UnrealThread::Invoke<bool>([&]() {
			lHand = adapter->GetLeftJointInfos();
			rHand = adapter->GetRightJointInfos();
			return true;
		}, false);

		// if hand data tracked, convert to grpc response
		if (isHandsTracked) {
			// extract positional and rotational data to arrays
			TArray<FVector> positions;
			TArray<FQuat> rotations;
			for (auto& i : lHand) {
				positions.Add(i.LocalPosition);
				rotations.Add(i.LocalRotation);
			}

			UAnchorServiceComponent::WorldToAnchorSpace(positions);
			UAnchorServiceComponent::WorldToAnchorSpace(rotations);

			// add positions & rotations to response left hand

			auto lHandResponse = new HandInfoResponse();
			lHandResponse->set_handedness(AR51::HandednessTypes::LeftHand);
			lHandResponse->set_positions(UnityAdapter::ToUnityBytestring(positions, UnityAdapter::TO_UNITY_UNIT_SCALE));
			lHandResponse->set_rotations(UnityAdapter::ToUnityBytestring(rotations, UnityAdapter::TO_UNITY_UNIT_SCALE));
			lHandResponse->set_istracked(positions.Num() > 0);
			response.set_allocated_lefthandinfo(lHandResponse);
			
			positions.Empty();
			rotations.Empty();
			for (auto& i : rHand) {
				positions.Add(i.Position);
				rotations.Add(i.Rotation);
			}

			// add positions & rotations to response right hand
			auto rHandResponse = new HandInfoResponse();
			rHandResponse->set_handedness(AR51::HandednessTypes::RightHand);
			rHandResponse->set_istracked(positions.Num() > 0);
			rHandResponse->set_positions(UnityAdapter::ToUnityBytestring(positions, UnityAdapter::TO_UNITY_UNIT_SCALE));
			rHandResponse->set_rotations(UnityAdapter::ToUnityBytestring(rotations, UnityAdapter::TO_UNITY_UNIT_SCALE));
			response.set_allocated_righthandinfo(rHandResponse);
		}
		else {
			status = Status::CANCELLED;
			break;
		}
	} while (writer->Write(response));

	return status;
}
