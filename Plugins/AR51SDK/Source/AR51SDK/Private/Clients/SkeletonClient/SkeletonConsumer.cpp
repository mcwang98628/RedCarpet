#include "SkeletonConsumer.h"

#include "unreal.grpc.h"

#include "Converter.h"
#include "Debug.h"
#include "UnrealThread.h"
#include "Keypoints.h"
#include "Person.h"
#include "AR51UnrealSDK.h"
#include "RegistrationClient.hpp"
#include <Kismet\GameplayStatics.h>
#include <UnityAdapter.h>
#include <AnchorServiceComponent.h>
#include <HandsAdapterFactory.h>
#include <fstream>
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include <fstream>

// Sets default values for this component's properties

#include <grpcpp/grpcpp.h>
#include <thread>
#include "BoneTransform.h"
#include "HandJointType.h"

class AsyncHandSender {
public:
	AsyncHandSender(const FString& endpoint)
		: _stub(AR51::SkeletonService::NewStub(grpc::CreateChannel(ToString(endpoint), grpc::InsecureChannelCredentials()))) {
		// Spawn a new thread to handle the completion queue
		_cqThread = std::thread(&AsyncHandSender::CompletionQueueThread, this);
	}

	~AsyncHandSender() {
		// Shutdown the completion queue
		_cq.Shutdown();
		// Join the thread
		if (_cqThread.joinable()) {
			_cqThread.join();
		}
	}

	void SendAsync(const AR51::HandsInfoRequest& request) {
		auto* context = new grpc::ClientContext();
		auto* response = new AR51::Empty();

		// Prepare the call
		auto rpc = _stub->PrepareAsyncSendHands(context, request, &_cq);

		// Begin the call and delete it once it finishes
		rpc->StartCall();
		rpc->Finish(response, &_status, (void*)context);
	}

private:
	void CompletionQueueThread() {
		void* tag;
		bool ok;
		while (_cq.Next(&tag, &ok)) {
			// The call is finished, so we can delete the context
			delete static_cast<grpc::ClientContext*>(tag);
		}
	}

	std::unique_ptr<AR51::SkeletonService::Stub> _stub;
	grpc::CompletionQueue _cq;
	std::thread _cqThread;
	grpc::Status _status;
};



USkeletonConsumer::USkeletonConsumer() { PrimaryComponentTick.bCanEverTick = true; }
USkeletonConsumer::~USkeletonConsumer() { 
	if (IsConnected()) Disconnect(); 
	if (IsRecording) StopRecording();
	if (IsPlayback) StopPlayback();
}


void USkeletonConsumer::DrawControllers(const TArray<FVector>& positions) {

	if (!ShowControllers) return;
	
	for (auto p : positions) {
		_controllersIsm->AddInstance(FTransform(FQuat::Identity, p, FVector::OneVector * ControllerSize));
	}
}

void USkeletonConsumer::DrawSkeleton(const TArray<FVector>& positions) {
	if (!ShowSkeleton) return;

	static const std::vector<FIntPoint> connections = {
		{(int)Keypoints::MidHip, (int)Keypoints::Neck},
		{(int)Keypoints::Neck, (int)Keypoints::Nose},

		{(int)Keypoints::Neck, (int)Keypoints::RShoulder},
		{(int)Keypoints::RShoulder, (int)Keypoints::RElbow},
		{(int)Keypoints::RElbow, (int)Keypoints::RWrist},

		{(int)Keypoints::Neck, (int)Keypoints::LShoulder},
		{(int)Keypoints::LShoulder, (int)Keypoints::LElbow},
		{(int)Keypoints::LElbow, (int)Keypoints::LWrist},

		{(int)Keypoints::MidHip, (int)Keypoints::RHip},
		{(int)Keypoints::RHip, (int)Keypoints::RKnee},
		{(int)Keypoints::RKnee, (int)Keypoints::RAnkle},
		{(int)Keypoints::RAnkle, (int)Keypoints::RBigToe},

		{(int)Keypoints::MidHip, (int)Keypoints::LHip},
		{(int)Keypoints::LHip, (int)Keypoints::LKnee},
		{(int)Keypoints::LKnee, (int)Keypoints::LAnkle},
		{(int)Keypoints::LAnkle, (int)Keypoints::LBigToe},
	};


	for (auto c : connections) {
		auto from = positions[c.X];
		auto to = positions[c.Y];
		auto center = 0.5f * (from + to);
		auto dir = to - from;
		auto length = dir.Size();


		auto up = dir.GetSafeNormal();
		auto forward = FVector::DotProduct(up, FVector::ForwardVector) < .99f ? FVector::ForwardVector : FVector::UpVector;
		auto right = FVector::CrossProduct(up, forward).GetSafeNormal();
		forward = FVector::CrossProduct(up, right).GetSafeNormal();

		FMatrix r(right, forward, up, FVector::ZeroVector);
		_skeletonIsm->AddInstance(FTransform(r.ToQuat(), from, { SkeletonSize, SkeletonSize, length / 100.f }));
	}
}

std::shared_ptr<Person> USkeletonConsumer::CreatePerson(AR51::SkeletonReply& skeleton) {
	if (CharacterBlueprints.Num() == 0) {
		LOG("Error: could not create a person. CharacterBlueprints is empty!");
		return nullptr;
	}

	auto id = ToFString(skeleton.id());
	auto  bpName = ToFString(skeleton.characterprefab());
	auto bpIndex = FindCharacterIndexByName(bpName);
	auto bp = bpIndex >= 0 ? CharacterBlueprints[bpIndex] : CharacterBlueprints[0];

	auto model = UnityAdapter::Instantiate(GetOwner(), bp);
	auto character = UnityAdapter::GetComponent<UAR51Character>(model);
	if (character)
	{
		auto person = std::make_shared<Person>(id, character);
		character->SetSkeletonId(id);
		_personById.Add(id, person);

		// todo invoke / broadcast event with the arguemnt of which character is created.
		//OnCharacterCreated.Broadcast(character);
		return person;
	}
	// todo warning, non AR51Character object
	return nullptr;
}

void USkeletonConsumer::DestroyPerson(std::shared_ptr<Person>& person) {

	// todo invoke / broadcast event with the arguemnt of which character is created.
	//OnCharacterBeingDestroyed.Broadcast(person->GetCharacter());
	
	_personById.Remove(person->GetId());
	person->GetModel()->Destroy();
}

/// <summary>
/// Finds the index of a character blueprint within the CharacterBlueprints array using the blueprint's name.
/// </summary>
/// <param name="CharacterBPName">The name of the character blueprint to find.</param>
/// <returns>Returns the index of the character blueprint if found, -1 otherwise.</returns>
int32 USkeletonConsumer::FindCharacterIndexByName(const FString& CharacterBPName)
{
	// Iterate over all blueprints in the CharacterBlueprints array
	for (int32 i = 0; i < CharacterBlueprints.Num(); i++)
	{
		// Get the name of the current blueprint
		FString BlueprintName = CharacterBlueprints[i].Get()->GetName();

		// If the name of the current blueprint matches the input string...
		if (BlueprintName == CharacterBPName)
		{
			// Return the current index
			return i;
		}
	}

	// If no matching blueprint is found, return -1
	return -1;
}

/// <summary>
/// Removes a character from the _personById map and destroys its model.
/// </summary>
/// <param name="id">The unique identifier of the character to be reset.</param>
void USkeletonConsumer::ResetCharacter(const FString& id)
{
	if (!UnrealThread::IsMainThread()) {
		UnrealThread::BeginInvoke([this, id]() { ResetCharacter(id); }, "ResetCharacter");
		return;
	}

	auto personPtr = _personById.Find(id);

	if (personPtr) {
		auto person = *personPtr;

		_personById.Remove(person->GetId());
		person->GetModel()->Destroy();
	}
}

/// <summary>
/// Destroys all the models of characters in the _personById map and then clears the map.
/// </summary>
void USkeletonConsumer::ResetCharacters() {
	if (!UnrealThread::IsMainThread()) {
		UnrealThread::BeginInvoke([this]() { ResetCharacters(); }, "ResetCharacters");
		return;
	}
	for (auto& p : _personById) {
		if (p.Value && p.Value->GetModel() && p.Value->GetModel()->IsValidLowLevel())
			p.Value->GetModel()->Destroy();
	}
	_personById.Reset();
}

/// <summary>
/// Sets the active person's character using a character blueprint name.
/// </summary>
/// <param name="CharacterPrefabName">The name of the character prefab to be set as active.</param>
/// <returns>Returns true if successful, false otherwise.</returns>
bool USkeletonConsumer::SetActiveCharacter(const FString& characterBPName)
{
	// Check if the CharacterPrefabs array is valid and not empty
	if (CharacterBlueprints.Num() <= 0)
	{
		LOG("Error: Could not set active person's character, Characters blueprints is empty.");
		return false;
	}

	// Find the index of the character in the CharacterPrefabs array by name
	int32 Index = FindCharacterIndexByName(characterBPName);

	// If the character is not found, return false
	if (Index < 0)
	{
		LOG("Error: Could not set active person's character to %s, blueprint with that name could not be found!", *characterBPName);
		return false;
	}

	// Set the active character prefab index and the active character prefab
	_activeCharacterPrefabIndex = Index;
	_activeCharacterBP = CharacterBlueprints[Index];

	// Reset the character with the given id
	if (_activePerson) {
		ResetCharacter(_activePerson->GetId());
	}
	else{
		LOG("Warning: could not reset active person character, active person is null.");
	}

	return true;
}

TArray<UAR51Character*> USkeletonConsumer::GetCharacters() const {
	TArray<UAR51Character*> characters;
	for (auto& pair : _personById) {
		characters.Add(pair.Value->GetCharacter());
	}
	return characters;
}

void USkeletonConsumer::Record(const AR51::SkeletonReply& skeleton) {

	std::string serialized;
	skeleton.SerializeToString(&serialized);

	// Convert the std::string to an Unreal TArray<uint8>
	TArray<uint8> BinaryData;
	BinaryData.Append((uint8*)serialized.data(), serialized.size());

	// Base64 encode
	FString EncodedString = FBase64::Encode(BinaryData);

	// Convert to std::string and write to the file
	_recordingOutputFile << TCHAR_TO_UTF8(*EncodedString) << std::endl;
}

void USkeletonConsumer::OnSkeleton(AR51::SkeletonReply skeleton) {
	if (IsRecording) Record(skeleton);

	auto id = ToFString(skeleton.id());
	auto timestamp = GetWorld()->GetTimeSeconds();
	auto person = _personById.FindRef(id);

	if (!person) {
		person = CreatePerson(skeleton);

		if (!person) {
			LOG("Warning: Could not update skeleton, person is null. Check CreatePerson");
			return;
		}
	}


	if (person) {

		person->SetLastSkeleton(skeleton, timestamp);
		person->FixCalfRatio(CalfRatio);

		UnityAdapter::SetActive(person->GetModel(), true);
	}
	else {
		LOG("Error: person is null.");
	}
}

void USkeletonConsumer::ConsumeSkeletonLoop() {

	LOG("SkeletonConsumer starting consume loop...");

	AR51::Empty request;
	AR51::SkeletonReply response;
	auto reader = _stub->Register(_context.get(), request);

	while (reader->Read(&response)) {
		auto taskName = FString::Printf(TEXT("SkeletonConsumer::OnSkeleton %s"), *ToFString(response.id()));

		if(!IsPlayback) UnrealThread::BeginInvoke([this, response]() { OnSkeleton(response); }, taskName);
	}

	LOG("SkeletonConsumer consume loop completed successfully.");
}


bool USkeletonConsumer::IsConnected() { return _isConnected; }

void USkeletonConsumer::Connect(const FString& endpoint) {
	if (IsConnected()) {
		LOG("Error: Skeleton client cannot connect while is connected to %s.", *_endpoint);
		return;
	}

	_isConnected = true;
	_endpoint = endpoint;
	_context = std::make_unique<grpc::ClientContext>();
	_stub = AR51::SkeletonService::NewStub(grpc::CreateChannel(ToString(endpoint), grpc::InsecureChannelCredentials()));
	_asyncHandSender = std::make_shared<AsyncHandSender>(endpoint);

	GEngine->ForceGarbageCollection(true); //solves a unreal-crach-bug that is caused by an object with the same name on a system re-connect

	_consumerThread = std::thread([this]() { ConsumeSkeletonLoop(); });
}

void USkeletonConsumer::Disconnect() {
	ResetCharacters();
	if (!IsConnected()) {
		LOG("Error: Skeleton client cannot disconnect while not connected!");
		return;
	}

	_isConnected = false;
	if (_context) _context->TryCancel();
	if (_consumerThread.joinable()) _consumerThread.join();
	_context.reset();
	_stub.reset();

	if (_asyncHandSender) _asyncHandSender.reset();
	GEngine->ForceGarbageCollection(true); //solves a unreal-crach-bug that is caused by an object with the same name on a system re-connect

}



void USkeletonConsumer::InitVisualization() {
	if (_controllersIsm) return;

	_controllersIsm = UnityAdapter::AddComponent<UInstancedStaticMeshComponent>(GetOwner());
	if (_controllersIsm) {
		_controllersIsm->SetupAttachment(GetOwner()->GetRootComponent());
		_controllersIsm->SetStaticMesh(ControllerMesh);
		_controllersIsm->SetFlags(RF_Transactional);
	}
	else {
		LOG("Error: Failed creating ControllersMesh");
	}

	_skeletonIsm = UnityAdapter::AddComponent<UInstancedStaticMeshComponent>(GetOwner());
	if (_skeletonIsm) {
		_skeletonIsm->SetupAttachment(GetOwner()->GetRootComponent());
		_skeletonIsm->SetStaticMesh(SkeletonMesh);
		_skeletonIsm->SetFlags(RF_Transactional);
	}
	else {
		LOG("Error: Failed creating SkeletonLinesMesh");
	}
}

void USkeletonConsumer::InitRegistration() {
	if (!AAR51SDK::Instance()) {
		LOG("Error: AR51SDK instance is null.");
		return;
	}

	auto reg = AAR51SDK::Instance()->GetRegistration();

	reg->OnComponentAdded += [this](void* sender, const AR51::ComponentDescriptor& component) {
		if (component.type() == AR51::CVSComponent) {
			auto endpoint = Converter::GetEndpoint(component.ip(), component.port());
			Connect(endpoint);
		}
	};

	reg->OnComponentRemoved += [this](void* sender, const AR51::ComponentDescriptor& component) {
		if (component.type() == AR51::CVSComponent) {
			Disconnect();
		}
	};
}

void USkeletonConsumer::Visualize() {
	if (!_controllersIsm || !_skeletonIsm) {
		LOG("Error: cannot visualize.");
		return;
	}

	if (_controllersIsm->GetInstanceCount())
		_controllersIsm->ClearInstances();

	if (_skeletonIsm->GetInstanceCount())
		_skeletonIsm->ClearInstances();

	for (auto& p : _personById) {
		auto& person = p.Value;
		DrawControllers(person->GetRawPositions());
		DrawSkeleton(person->GetRawPositions());
		UnityAdapter::SetVisiblity(person->GetModel(), ShowModel);
	}
}



void USkeletonConsumer::SolveIK()
{
	for (auto& p : _personById) {
		auto& person = p.Value;
		auto positions = person->GetRawPositions();
		auto character = person->GetCharacter();
		character->Solve(positions);
	}
}

void USkeletonConsumer::SetHandsFromAdapter()
{
	auto adapter = HandsAdapterFactory::GetAdapter();
	if (!adapter || !_activePerson) return;

	auto leftHand = adapter->GetLeftJointInfos();
	auto rightHand = adapter->GetRightJointInfos();

	auto leftHandPositions = HandJointInfo::GetPositions(leftHand);
	auto rightHandPositions = HandJointInfo::GetPositions(rightHand);

	_activePerson->GetCharacter()->SolveHands(leftHandPositions, rightHandPositions);
}

void USkeletonConsumer::CopyHeadRotationFromXRCamera()
{
	if (!_activePerson) return;

	if (!UnityAdapter::GetMainCamera()) {
		LOG("Warning: Main camera could not be found.");
		return;
	}

	auto p = AAR51SDK::Instance()->Platform;
	switch (p)
	{
	case EPlatformTypes::HoloLens:
	case EPlatformTypes::HoloLens2:
	case EPlatformTypes::HtcVive:
	case EPlatformTypes::OculusQuest:
	case EPlatformTypes::OculusRift:
		{
			auto cam = UnityAdapter::GetMainCameraTransform();
			_activePerson->GetCharacter()->SetHeadRotation(cam.GetRotation());
		}
		break;
	default:
		break;
	}
}

Person* USkeletonConsumer::GetActivePerson() {
	return _activePerson;
}

void USkeletonConsumer::SetActivePerson(Person* person) {
	_activePerson = person;
}

void USkeletonConsumer::UpdateActivePerson()
{
	if (!AAR51SDK::Instance()|| AAR51SDK::Instance()->Platform == EPlatformTypes::PC || AAR51SDK::Instance()->IsDGS)
	{
		SetActivePerson(nullptr);
		return;
	}

	if (!UnityAdapter::GetMainCamera()) {
		LOG("Warning: no camera in level.");
		return;
	}

	auto cam = UnityAdapter::GetMainCameraTransform();
	auto camPos = cam.GetPosition();

	Person* bestCandidate = nullptr;
	auto minDistance = 1e10f;
	for (auto& p : _personById) {
		auto dist = (p.Value->GetCharacter()->GetHeadPosition() - camPos).Size();

		if (dist < minDistance)
		{
			minDistance = dist;
			bestCandidate = p.Value.get();
		}
	}

	if (bestCandidate  && (bestCandidate->GetCharacter()->GetHeadPosition() - camPos).Size() < MaxCameraDistanceThreshold)
	{
		SetActivePerson(bestCandidate);
	}
	else
	{
		SetActivePerson(nullptr);
	}
}

void USkeletonConsumer::CheckInactivePersons()
{
	if (!UpdateSkeleton) return;

	auto currentTime = GetWorld()->GetTimeSeconds();
	// Delete inactive persons
	if (DestroyInactivePersons)
	{
		TArray<std::shared_ptr<Person>> inactivePersons;
		for (auto& p : _personById) {
			if (p.Value->GetTimeFromLastUpdate(currentTime) > InactivePersonMaxSeconds)
				inactivePersons.Add(p.Value);
		}

		for (auto& p : inactivePersons) {
			
			DestroyPerson(p);
		}
	}

	// Hide inactive persons
	for (auto& p : _personById) {
		if (p.Value->GetLastUpdateTime() > HidePersonMaxSeconds && UnityAdapter::GetVisiblity(p.Value->GetCharacter()->GetOwner())) {
			UnityAdapter::SetVisiblity(p.Value->GetCharacter()->GetOwner(), false, true);
		}
	}
}

void USkeletonConsumer::ApplyHeadRotationWristPositionAndFingerRotations()
{
	if (!IsApplyHeadRotationWristPositionAndFingerRotations) return;
	for(auto pById : _personById)
	{
		if (pById.Value.get() == _activePerson) continue;
		
		auto& person = *pById.Value;
		auto character = person.GetCharacter();

		auto& skeleton = person.GetLastSkeleton();
		//var anchor = GetAnchor(skeleton.AnchorId);

		auto lHandPos = Converter::ToTArray<FVector>(skeleton.lefthandpositions());
		auto rHandPos = Converter::ToTArray<FVector>(skeleton.righthandpositions());
		UnityAdapter::ToUnrealInplace(lHandPos, UnityAdapter::TO_UNREAL_UNIT_SCALE);
		UnityAdapter::ToUnrealInplace(rHandPos, UnityAdapter::TO_UNREAL_UNIT_SCALE);
		UAnchorServiceComponent::AnchorToWorldSpace(lHandPos);
		UAnchorServiceComponent::AnchorToWorldSpace(rHandPos);
		//if (skeleton.HasWristPositions)
		//{
		//    var pos = TransformPositions(skeleton.AnchorId, skeleton.LeftWristPosition.ToUnity(), skeleton.RightWristPosition.ToUnity());
		//    person.LeftWristIK.position = pos[0];
		//    person.RightWristIk.position = pos[1];
		//}

		character->SolveHands(lHandPos, rHandPos);

		if (skeleton.hasheadlocalrotation())
		{
			auto headRotation = UnityAdapter::ToUnreal(skeleton.headlocalrotation());
			UAnchorServiceComponent::AnchorToWorldSpace(headRotation); 
			character->SetHeadRotation(headRotation);
		}
	}
}

void USkeletonConsumer::SendHeadRotationWristIKPosAndFingerRotationsToCV()
{
	if (_activePerson && _stub)
	{
		// get local rotation for fingers and world rotation for wrist
		TArray<FQuat> leftHandRotations;
		TArray<FQuat> rightHandRotations;
		TArray<FVector> leftHandPositions;
		TArray<FVector> rightHandPositions;

		auto leftHandJoints = _activePerson->GetCharacter()->GetLeftHandJoints();
		for (auto i = 0; i < leftHandJoints.Num(); i++)
		{
			auto type = (EHandJointType)i;
			if (leftHandJoints[i]) {
				leftHandRotations.Add(leftHandJoints[i]->rotation());
				leftHandPositions.Add(leftHandJoints[i]->position());
			}
			else if (HandJointTypeHelper::IsTip(type) && leftHandJoints[i - 1]) {
				auto pos = leftHandJoints[i - 1]->localToWorldMatrix().TransformPosition(leftHandJoints[i - 1]->localPosition());
				leftHandRotations.Add(FQuat::Identity);
				leftHandPositions.Add(pos);
			}
			else {
				LOG("Warning: Missing left hand joint %s", *HandJointTypeHelper::ToString((EHandJointType)i));

				leftHandRotations.Add(FQuat::Identity);
				leftHandPositions.Add(FVector::ZeroVector);
			}
		}
		UAnchorServiceComponent::WorldToAnchorSpace(leftHandRotations);
		UAnchorServiceComponent::WorldToAnchorSpace(leftHandPositions);

		auto rightHandJoints = _activePerson->GetCharacter()->GetRightHandJoints();
		for (auto i = 0; i < rightHandJoints.Num(); i++)
		{
			auto type = (EHandJointType)i;

			if (rightHandJoints[i]) {

				rightHandRotations.Add(rightHandJoints[i]->rotation());
				rightHandPositions.Add(rightHandJoints[i]->position());
			}
			else if (HandJointTypeHelper::IsTip(type) && rightHandJoints[i - 1]) {
				auto pos = rightHandJoints[i - 1]->localToWorldMatrix().TransformPosition(rightHandJoints[i - 1]->localPosition());
				rightHandRotations.Add(FQuat::Identity);
				rightHandPositions.Add(pos);
			}
			else {
				LOG("Warning: Missing right hand joint %s", *HandJointTypeHelper::ToString(type));
				rightHandRotations.Add(FQuat::Identity);
				rightHandPositions.Add(FVector::ZeroVector);
			}
		}
		UAnchorServiceComponent::WorldToAnchorSpace(rightHandRotations);
		UAnchorServiceComponent::WorldToAnchorSpace(rightHandPositions);

		grpc::ClientContext context;
		AR51::HandsInfoRequest request;

		request.set_skeletonid(_activePerson->GetLastSkeleton().id());

		auto deviceId = OverrideDeviceId.IsEmpty() ?
			AAR51SDK::Instance()->GetRegistration()->GetDeviceId() : OverrideDeviceId;
		request.set_deviceid(ToString(deviceId));

		auto personHeadRotation = _activePerson->GetCharacter()->GetHeadRotation();
		if (GEngine && GEngine->XRSystem && GEngine->XRSystem.IsValid()) {
			if (UnityAdapter::GetMainCamera())
				personHeadRotation = UnityAdapter::GetMainCameraTransform().GetRotation();
			else
				LOG("Warning: Main camera is null, sending identity rotation.");
		}
		UAnchorServiceComponent::WorldToAnchorSpace(personHeadRotation);
		auto headLocalRotation = UnityAdapter::ToUnity(personHeadRotation);

		
		AR51::Quaternion* hearRotationRequest = request.mutable_headlocalrotation(); 
		hearRotationRequest->set_x(headLocalRotation.X);
		hearRotationRequest->set_y(headLocalRotation.Y);
		hearRotationRequest->set_z(headLocalRotation.Z);
		hearRotationRequest->set_w(headLocalRotation.W);

		auto lWristPos = _activePerson->GetCharacter()->GetLeftWristPosition();
		UAnchorServiceComponent::WorldToAnchorSpace(lWristPos);
		
		lWristPos = UnityAdapter::ToUnity(lWristPos, UnityAdapter::TO_UNITY_UNIT_SCALE);
		auto lWristPosRequest = request.mutable_leftwristposition();
		lWristPosRequest->set_x(lWristPos.X);
		lWristPosRequest->set_y(lWristPos.Y);
		lWristPosRequest->set_z(lWristPos.Z);
		request.set_lefthandpositions(UnityAdapter::ToUnityBytestring(leftHandPositions, UnityAdapter::TO_UNITY_UNIT_SCALE));
		request.set_lefthandrotation(UnityAdapter::ToUnityBytestring(leftHandRotations));

		auto rWristPos = _activePerson->GetCharacter()->GetRightWristPosition();
		UAnchorServiceComponent::WorldToAnchorSpace(rWristPos);
		rWristPos = UnityAdapter::ToUnity(rWristPos, UnityAdapter::TO_UNITY_UNIT_SCALE);
		auto rWristPosRequest = request.mutable_rightwristposition();
		rWristPosRequest->set_x(rWristPos.X);
		rWristPosRequest->set_y(rWristPos.Y);
		rWristPosRequest->set_z(rWristPos.Z);
		request.set_righthandpositions(UnityAdapter::ToUnityBytestring(rightHandPositions, UnityAdapter::TO_UNITY_UNIT_SCALE));
		request.set_righthandrotation(UnityAdapter::ToUnityBytestring(rightHandRotations));
		
		_asyncHandSender->SendAsync(request);
	}
}

void USkeletonConsumer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsPlayback) StopPlayback();
	
	if (IsRecording) StopRecording();
}

void USkeletonConsumer::StopRecording() {

	IsRecording = false;
	_recordingOutputFile.close();
}

bool USkeletonConsumer::StartRecording() {
	auto inOutRecordingFullPath = FPaths::IsRelative(InOutRecordingFile) ? FPaths::ProjectDir() + InOutRecordingFile : InOutRecordingFile;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Extract the directory path
	FString Directory = FPaths::GetPath(inOutRecordingFullPath);

	// Check if directory exists
	if (!PlatformFile.DirectoryExists(*Directory))
	{
		// If the directory doesn't exist, create it
		PlatformFile.CreateDirectory(*Directory);

		// Double check if the directory was successfully created
		if (!PlatformFile.DirectoryExists(*Directory))
		{
			// Log an error or handle appropriately
			LOG("Failed to create directory: %s", *Directory);
			return false;
		}
	}

	// Convert FilePath to the necessary string format for std::ofstream
	std::string stdFilePath(TCHAR_TO_UTF8(*inOutRecordingFullPath));

	// Now, try to open the file using std::ofstream
	_recordingOutputFile.open(stdFilePath, std::ios::out | std::ios::binary);  // Assuming binary mode

	if (!_recordingOutputFile.is_open() || !_recordingOutputFile.good())
	{
		// Handle the error - the file failed to open for writing
		LOG("Error: Failed to open file for writing: %s", *inOutRecordingFullPath);
		return false;
	}

	LOG("Starting recording to path: %s", *inOutRecordingFullPath);
	// If we reach here, everything went well
	return true;
}

void USkeletonConsumer::StopPlayback() {
	IsPlayback = false;
	if(_playbackThread.joinable()) _playbackThread.join();
}

void USkeletonConsumer::StartPlayback() 
{
	auto inOutRecordingFullPath = FPaths::IsRelative(InOutRecordingFile) ? FPaths::ProjectDir() + InOutRecordingFile : InOutRecordingFile;

	// validate the file exists
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*inOutRecordingFullPath)) {

		LOG("Error: Missing file. Cannot start playback from path: %s", *inOutRecordingFullPath);
		IsPlayback = false;
		return;
	}

	// start playback loop
	_playbackThread = std::thread([&, inOutRecordingFullPath] {
		do {


			std::string line;
			std::ifstream playbackFile;

			LOG("Starting playback from path: %s", *inOutRecordingFullPath);

			playbackFile.open(ToString(inOutRecordingFullPath), std::ios::out);

			AR51::SkeletonReply skeleton;
			double prevCaptureTime = -1;
			while (IsPlayback && std::getline(playbackFile, line))
			{
				// Process the read message - e.g., print to console or other action
				LOG("Playing back message: %s", *ToFString(line));


				FString DecodedFString = UTF8_TO_TCHAR(line.c_str());

				// Base64 decode
				TArray<uint8> DecodedData;
				FBase64::Decode(DecodedFString, DecodedData);

				// Convert TArray<uint8> back to std::string
				std::string DecodedString((char*)DecodedData.GetData(), DecodedData.Num());

				skeleton.ParseFromString(DecodedString);


				if (prevCaptureTime < 0) prevCaptureTime = skeleton.capturetime();

				// Adding a sleep to simulate some time delay. Adjust as necessary.
				auto delaySeconds = (skeleton.capturetime() - prevCaptureTime);
				if (delaySeconds > 0)
					FPlatformProcess::Sleep(delaySeconds);

				prevCaptureTime = skeleton.capturetime();
				UnrealThread::BeginInvoke([skeleton, this]() { OnSkeleton(skeleton); });
			}

			playbackFile.close();
		} while (IsPlayback && IsPlaybackLoop);
	});
}

void USkeletonConsumer::BeginPlay()
{
	Super::BeginPlay();

	LOG("Start playing...");
	if (!_isInitialized) {
		_isInitialized = true;
		LOG("Initializing...");
		InitVisualization();
		InitRegistration();
	}


	auto inOutRecordingFullPath = FPaths::ProjectDir() + InOutRecordingFile;

	if (IsRecording) {
		StartRecording();
	}
	else if (IsPlayback) {
		StartPlayback();
	}
}

void USkeletonConsumer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Print warnings of person's configrations.
	for (auto& p : _personById) {
		if (p.Value->GetCharacter() && !p.Value->GetCharacter()->SkeletalMesh)
			UE_LOG(LogTemp, Log, TEXT("Warnning: Person %s\'s SkeletalMesh is none."), *p.Key);
	}
	CheckInactivePersons();

	SolveIK();
	UpdateActivePerson();
	SetHandsFromAdapter();
	CopyHeadRotationFromXRCamera();

	ApplyHeadRotationWristPositionAndFingerRotations();
	SendHeadRotationWristIKPosAndFingerRotationsToCV();

	// todo where to disable Visualize?
	Visualize();
//	OnSkeletonUpdated.Broadcast();
}

bool FSolverParams::IsValidScale(float scale) {
	auto normalizedScale = scale;
	if (AAR51SDK::Instance()) {
		TTransform sdk(AAR51SDK::Instance());
		normalizedScale /= sdk.GetLossyScale().X;
	}
	return MinValidScale <= normalizedScale && normalizedScale <= MaxValidScale;
}
