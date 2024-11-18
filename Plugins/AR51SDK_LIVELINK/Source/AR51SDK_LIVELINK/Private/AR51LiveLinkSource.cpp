#include "AR51LiveLinkSource.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/FileManagerGeneric.h"
#include "ILiveLinkClient.h"
#include <LiveLinkController.h>
#include <LiveLinkVirtualSubject.h>
#include <LiveLinkAnimationVirtualSubject.h>
#include <AR51Character.h>
#include <AR51LiveLinkSubject.h>

#define LOCTEXT_NAMESPACE "AR51LiveLinkSource"

static bool IsInPlayMode()
{
	if (!GEngine) return false;

	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		UWorld* World = Context.World();
		if (!World || World->bIsTearingDown) continue;

		if (World->WorldType == EWorldType::PIE || World->WorldType == EWorldType::Game)
		{
			return true;
		}
	}

	return false;
}

/* Constructor and Destructor */
/* ========================== */
FAR51LiveLinkSource::FAR51LiveLinkSource() {}
FAR51LiveLinkSource::~FAR51LiveLinkSource() { RequestSourceShutdown(); }


/* ILiveLinkSource interface implementation */
/* ======================================== */
void FAR51LiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	_liveLinkClient = InClient;
	_liveLinkSourceGuid = InSourceGuid;
}
void FAR51LiveLinkSource::InitializeSettings(ULiveLinkSourceSettings* Settings)
{
	_settings = Cast<UAR51LiveLinkSourceSettings>(Settings);
}
bool FAR51LiveLinkSource::RequestSourceShutdown()
{
	ClearVirtualSubjects();
	RemoveSourceSubjects();
	if (_liveLinkClient)
		_liveLinkClient->RemoveSource(_liveLinkSourceGuid);
	_liveLinkSourceGuid.Invalidate();
	_liveLinkClient = nullptr;
	return true;
}

bool FAR51LiveLinkSource::IsSourceStillValid() const { return _liveLinkClient != nullptr; }
void FAR51LiveLinkSource::Update()
{
	if (IsInPlayMode() && USkeletonConsumer::Instance()) {
		if (_settings) USkeletonConsumer::Instance()->ShowModel = _settings->ShowModel;
		UpdateLiveLink();
	}
	else
	{
		ClearVirtualSubjects();
		RemoveSourceSubjects();
	}
}

FText FAR51LiveLinkSource::GetSourceType() const 
{
	return LOCTEXT("AR51LiveLinkSourceType", "AR51 LiveLink");
}
FText FAR51LiveLinkSource::GetSourceMachineName() const 
{
	FText name = FText::FromString(TEXT("NotConnected"));

	if (!IsInPlayMode()) return name;


	if (USkeletonConsumer::Instance() && !USkeletonConsumer::Instance()->GetEndpoint().IsEmpty())
		name = FText::FromString(USkeletonConsumer::Instance()->GetEndpoint());

	return name;
}
FText FAR51LiveLinkSource::GetSourceStatus() const 
{
	if (!IsInPlayMode()) return FText::FromString("Not Connected");
	auto isConnected = USkeletonConsumer::Instance() && USkeletonConsumer::Instance()->IsConnected();
	auto status = isConnected ? "Connected" : "Not Connected";
	return FText::FromString(status);
}


void FAR51LiveLinkSource::UpdateLiveLink()
{
	if (!IsInPlayMode()) return;

	// Check if we're running in the Game Thread
	if (!IsInGameThread()) {
		LOG("Error: IsInGameThread is false.");
		return;
	}
	if (!_liveLinkClient) {
		LOG("Warning: LiveLinkClient is null.");
		return;
	}

	if (!USkeletonConsumer::Instance()) {
		LOG("Warning: USkeletonConsumer is null.");
		return;
	}

	// List existing subjects, add a new person if subject is new.
	TSet<FName> existingSubjects;
	for (auto character : USkeletonConsumer::Instance()->GetCharacters()) {
		auto id = character->GetSkeletonId();
		FName subjectName(id);

		existingSubjects.Add(subjectName);
		if (!_subjectNames.Contains(subjectName))
			PersonAdded(character);

		// update existing subject
		TArray<FName> boneNames;
		character->GetBoneNames(boneNames);
		TArray<FTransform> transforms;
		for (int i = 0; i < boneNames.Num(); i++)
		{
			auto T = character->GetBoneLocalTransformByName(boneNames[i]);
			transforms.Add(T);
		}

		FLiveLinkFrameData frameData;
		frameData.Transforms = transforms;
		frameData.WorldTime = FPlatformTime::Seconds();

		_liveLinkClient->PushSubjectData(_liveLinkSourceGuid, subjectName, frameData);
	}

	// Remove tracked subjects that no longer exist in the SDK.
	bool subjectRemoved = false;
	TSet<FName> newSubjects;
	for (auto activeSubject : _subjectNames) {
		if (existingSubjects.Contains(activeSubject))
		{
			newSubjects.Add(activeSubject);
			continue;
		}
		subjectRemoved = true;
		RemoveSubject(activeSubject);
	}
	_subjectNames = newSubjects;

	// Auto assign new Persons to virtual subjects by order.
	if (subjectRemoved)
		AutoAssignSubjects();
}


/* Person managing methods */
/* ======================= */
// Assign 'NewSubjectName' subject to an 'Actor''s variable of 'VariableName'.
// Used to assign the newly created subjects to the 'AR51Character' they correlate to.
void SetPropertySubjectName(AActor* Actor, FName VariableName, FLiveLinkSubjectName NewSubjectName)
{
	if (Actor)
	{
		// Find the class and property
		UClass* Class = Actor->GetClass();
		FProperty* prop = Class->FindPropertyByName(VariableName);
		if (prop)
		{
			FStructProperty* structProp = CastField<FStructProperty>(prop);
			if (structProp && structProp->Struct == TBaseStructure<FLiveLinkSubjectName>::Get())
			{
				// Get a pointer to the property value in memory
				void* propAddress = structProp->ContainerPtrToValuePtr<void>(Actor);
				// Now safely set the new value
				if (propAddress)
				{
					FLiveLinkSubjectName* liveLinkSubjPtr = static_cast<FLiveLinkSubjectName*>(propAddress);
					*liveLinkSubjPtr = NewSubjectName; // Copy assignment of the new subject name
				}
			}
		}
	}
}
void FAR51LiveLinkSource::PersonAdded(UAR51Character* Character)
{
	auto id = Character->GetSkeletonId();
	FName subjectName(id);
	if (!_subjectNames.Contains(subjectName))
	{
		SetPropertySubjectName(Character->GetOwner(), "LiveLinkBodySubj", subjectName);
		// Create a new subject
		_subjectNames.Add(subjectName);
		TArray<FName> boneNames;
		Character->GetBoneNames(boneNames);
		TArray<int> boneParents;
		for (int i = 0; i < boneNames.Num(); i++)
		{
			auto parentName = Character->GetParentBone(boneNames[i]);
			auto parentIndex = Character->GetBoneIndex(parentName);
			boneParents.Add(parentIndex);
		}

		FLiveLinkRefSkeleton liveLinkRefSkeleton;
		liveLinkRefSkeleton.SetBoneNames(boneNames);
		liveLinkRefSkeleton.SetBoneParents(boneParents);
		_liveLinkClient->PushSubjectSkeleton(_liveLinkSourceGuid, subjectName, liveLinkRefSkeleton);

		AutoAssignSubjects();
	}
}
void FAR51LiveLinkSource::AutoAssignSubjects()
{
	// GetSubjects includes virtual subjects.
	TArray<FLiveLinkSubjectKey> personSubjects = _liveLinkClient->GetSubjects(false, true);
	int virtualSubjectIndex = 0;
	int countSubjects = 0;
	// Go over all source subjects and assign them to available AR51 Class virtual subjects.
	for (auto activeSubject : _subjectNames) {
		// Assign to next available 'UAR51LiveLinkSubject' virtual subject.
		while (countSubjects < personSubjects.Num())
		{
			if (SetAutoAssignVirtualSubject(personSubjects[countSubjects++], activeSubject))
				break;
		}
	}
	ClearVirtualSubjects(countSubjects);
}
void FAR51LiveLinkSource::ClearVirtualSubjects(int FromIndex)
{
	if (!_liveLinkClient)
		return;
	// GetSubjects includes virtual subjects.
	TArray<FLiveLinkSubjectKey> personSubjects = _liveLinkClient->GetSubjects(false, true);

	for (int i = FromIndex; i < personSubjects.Num(); i++)
		ClearVirtualSubject(personSubjects[i]);
}
void FAR51LiveLinkSource::RemoveSourceSubjects()
{
	if (_liveLinkClient)
	{
		for (auto subjectName : _subjectNames)
		{
			FLiveLinkSubjectKey key = FLiveLinkSubjectKey(_liveLinkSourceGuid, subjectName);
			_liveLinkClient->RemoveSubject_AnyThread(key);
		}
		_subjectNames.Empty();
	}

}


/* Virtual Subject updating methods */
/* ================================ */
void FAR51LiveLinkSource::RemoveSubject(FName SubjectName)
{
	if (_liveLinkClient)
	{
		FLiveLinkSubjectKey key = FLiveLinkSubjectKey(_liveLinkSourceGuid, SubjectName);
		_liveLinkClient->RemoveSubject_AnyThread(key);
	}
}
bool FAR51LiveLinkSource::SetAutoAssignVirtualSubject(const FLiveLinkSubjectKey& Key, FName& SubjectName)
{
	auto isVirtual = _liveLinkClient->IsVirtualSubject(Key);
	if (isVirtual)
	{
		auto virtualSubject = Cast<UAR51LiveLinkSubject>(_liveLinkClient->GetSubjectSettings(Key));
		if (virtualSubject)
		{
			virtualSubject->SetSubject(SubjectName);
			return true;
		}
	}
	return false;
}
void FAR51LiveLinkSource::ClearVirtualSubject(const FLiveLinkSubjectKey& Key)
{
	auto isVirtual = _liveLinkClient->IsVirtualSubject(Key);
	if (isVirtual)
	{
		auto virtualSubject = Cast<UAR51LiveLinkSubject>(_liveLinkClient->GetSubjectSettings(Key));
		if (virtualSubject)
			virtualSubject->ClearSubjects();
	}
}

#undef LOCTEXT_NAMESPACE