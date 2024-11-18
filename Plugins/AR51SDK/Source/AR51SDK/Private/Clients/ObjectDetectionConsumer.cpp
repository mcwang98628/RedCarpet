#include "ObjectDetectionConsumer.h"
#include "unreal.grpc.h"

#include "Converter.h"
#include "Debug.h"
#include "UnrealThread.h"
#include "AR51UnrealSDK.h"
#include "RegistrationClient.hpp"
#include <UnityAdapter.h>
#include <AnchorServiceComponent.h>
#include "Engine/Engine.h"

UObjectDetectionConsumer::UObjectDetectionConsumer()
{
	PrimaryComponentTick.bCanEverTick = true;

}

UObjectDetectionConsumer::~UObjectDetectionConsumer()
{
    if(IsConnected()) Disconnect();
}

void UObjectDetectionConsumer::PruneInactive()
{
    if (!USkeletonConsumer::Instance())
        return;
    // Delete inactive objects
    if (USkeletonConsumer::Instance()->DestroyInactivePersons)
    {
        TArray<UTrackedInstance*> TrackedObjects;
        _instanceById.GenerateValueArray(TrackedObjects);

        for (int32 i = TrackedObjects.Num() - 1; i >= 0; i--)
        {
            UTrackedInstance* TrackedObject = TrackedObjects[i];
            float TimeFromLastUpdate = GetWorld()->TimeSeconds - TrackedObject->LastUpdateTime;

            if (TimeFromLastUpdate > USkeletonConsumer::Instance()->HidePersonMaxSeconds)
            {
                // Hide marker that has not received an update for HidePersonMaxSeconds seconds
                TrackedObject->GetOwner()->SetActorHiddenInGame(true);
            }
            if (TimeFromLastUpdate > USkeletonConsumer::Instance()->InactivePersonMaxSeconds)
            {
                // Destroy marker that has not received an update for InactivePersonMaxSeconds seconds
                TrackedObject->GetOwner()->Destroy();
                _instanceById.Remove(TrackedObject->Id);
            }

            if (!ShowMarkers && TrackedObject->Type == EInstanceType::Marker) {
                TrackedObject->GetOwner()->Destroy();
                _instanceById.Remove(TrackedObject->Id);
            }
        }
    }
}

void UObjectDetectionConsumer::OnObjectDetected(AR51::ObjectDetectionReply e) {
    
    // update markers
 /*   if (ShowMarkers) {
        for (const auto& m : e.markers())
        {
            auto id = ToFString(m.id());
            UTrackedInstance** TrackedInstancePtr = _instanceById.Find(id);
            UTrackedInstance* TrackedInstance;

            if (!TrackedInstancePtr)
            {
                // Create the mesh component
                FActorSpawnParameters SpawnParams;
                SpawnParams.Name = FName(id);
                APawn* MyPawn = GetWorld()->SpawnActor<APawn>(APawn::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
#if WITH_EDITOR
                MyPawn->SetActorLabel(id);
#endif
                UStaticMeshComponent* Sphere = NewObject<UStaticMeshComponent>(MyPawn, SpawnParams.Name);
                Sphere->RegisterComponent();

                // Set the sphere mesh
                if (MarkerMesh) Sphere->SetStaticMesh(MarkerMesh);

                // Attach it to the root component
                MyPawn->SetRootComponent(Sphere);
                MyPawn->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);

                TrackedInstance = UnityAdapter::EnsureComponent<UTrackedInstance>(MyPawn);

                TrackedInstance->Id = id;
                TrackedInstance->Type = EInstanceType::Marker;
                TrackedInstance->ColorName = ToFString(m.type());


                if (MarkerMaterial)
                {
                    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(MarkerMaterial, this);
                    if (DynamicMaterial)
                    {
                        DynamicMaterial->SetVectorParameterValue("Color", UnityAdapter::ParseColor(TrackedInstance->ColorName)); // Set the desired color
                        Sphere->SetMaterial(0, DynamicMaterial);
                    }
                };

                _instanceById.Add(id, TrackedInstance);
            }
            else
            {
                TrackedInstance = *TrackedInstancePtr;
            }

            TrackedInstance->LastUpdateTime = GetWorld()->GetTimeSeconds();
            TrackedInstance->Radius = m.radius();

            UnityAdapter::SetVisiblity(TrackedInstance->GetOwner(), true);

            TTransform t(TrackedInstance->GetOwner());

            auto pos = UnityAdapter::ToUnreal(m.position(), UnityAdapter::TO_UNREAL_UNIT_SCALE);
            auto scale = USkeletonConsumer::Instance()->ControllerSize * FVector::OneVector * m.radius();
            t.SetLocalPosition(pos - FVector::UpVector * scale.X);
            t.SetLocalScale(scale);
        }
    }
    
    // update 6Dof objects
    for (const auto& m : e.items())
    {
        auto id = ToFString(m.id());
        UTrackedInstance** TrackedInstancePtr = _instanceById.Find(id);
        UTrackedInstance* TrackedInstance;

        if (!TrackedInstancePtr)
        {
            AActor* NewActor = nullptr;
            TSubclassOf<AActor>* Prefab = Prefabs.FindByPredicate([&](TSubclassOf<AActor>& Item)
            {
                return Item->GetName() == id;
            });

            if (Prefab)
            {
                NewActor = GetWorld()->SpawnActor<AActor>(*Prefab, FTransform());
            }
            else
            {
                LOG("Warning: Failed in OnObjectDetected. Could not find prefab named: %s defaulting to a cube", *id);

                FActorSpawnParameters SpawnParams;
                SpawnParams.Name = FName(id);
                APawn* MyPawn = GetWorld()->SpawnActor<APawn>(APawn::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
#if WITH_EDITOR
                MyPawn->SetActorLabel(id);
#endif
                UStaticMeshComponent* Cube = NewObject<UStaticMeshComponent>(MyPawn, SpawnParams.Name);
                Cube->RegisterComponent();

                // Set the Default Object Mesh
                if (DefaultObjectMesh) Cube->SetStaticMesh(DefaultObjectMesh);

                // Attach it to the root component
                MyPawn->SetRootComponent(Cube);
                MyPawn->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
                NewActor = MyPawn;
                MyPawn->GetRootComponent()->SetRelativeScale3D(FVector::OneVector * 0.2f);
            }
#if WITH_EDITOR
            NewActor->SetActorLabel(id);
#endif
            TrackedInstance = UnityAdapter::EnsureComponent<UTrackedInstance>(NewActor);
            if (!TrackedInstance)
            {
                TrackedInstance = NewObject<UTrackedInstance>(NewActor);
                TrackedInstance->RegisterComponent();
            }

            TrackedInstance->Id = id;
            TrackedInstance->Type = EInstanceType::Object;
            _instanceById.Add(id, TrackedInstance);
        }
        else
        {
            TrackedInstance = *TrackedInstancePtr;
        }

        TrackedInstance->LastUpdateTime = GetWorld()->GetTimeSeconds();
        UnityAdapter::SetVisiblity(TrackedInstance->GetOwner(), true);

        TTransform t(TrackedInstance->GetOwner());
        FVector Position = UnityAdapter::ToUnreal(m.position(), UnityAdapter::TO_UNREAL_UNIT_SCALE);
        FVector DeltaPos = Position - t.GetLocalPosition();

        if (DeltaPos.Size() < SmoothPositionalThreshold)
        {
            auto pos = FMath::Lerp(t.GetLocalPosition(), Position, 1 - SmoothPositionalFactor);
            t.SetLocalPosition(pos);
        }
        else
        {
            t.SetLocalPosition(Position);
        }

        FRotator Rotator(UnityAdapter::ToUnreal(m.rotation()));

        FRotator CurrentRotator(t.GetLocalRotation());

        if (FMath::Abs(FRotator::ClampAxis(CurrentRotator.Yaw - Rotator.Yaw)) < SmoothRotationAngleThreshold)
        {
            t.SetLocalRotation(FMath::Lerp(CurrentRotator, Rotator, 1 - SmoothRotationFactor).Quaternion());
        }
        else
        {
            t.SetLocalRotation(Rotator.Quaternion());
        }
    }*/
}

void UObjectDetectionConsumer::ConsumeObjectDetectionsLoop() {

	LOG("UObjectDetectionConsumer starting consume loop...");

	AR51::Empty request;
	AR51::ObjectDetectionReply response;
	auto reader = _stub->Register(_context.get(), request);
	static long int id = 0;

	while (reader->Read(&response)) {
		id++;
		auto taskName = FString::Printf(TEXT("ObjectDetectionReply::OnObjectDetected %s"), *FString::FromInt(id));
		UnrealThread::BeginInvoke([this, response]() { OnObjectDetected(response); }, taskName);
	}

	LOG("UObjectDetectionConsumer consume loop completed successfully.");
}


bool UObjectDetectionConsumer::IsConnected() { return _isConnected; }

inline TArray<UTrackedInstance*> UObjectDetectionConsumer::GetTrackedMarkers() const
{
	TArray<UTrackedInstance*> trackedMarkers;

	for (const TPair<FString, UTrackedInstance*>& pair : _instanceById)
	{
		if (pair.Value && pair.Value->Type == EInstanceType::Marker)
		{
			trackedMarkers.Add(pair.Value);
		}
	}

	return trackedMarkers;
}

inline TArray<UTrackedInstance*> UObjectDetectionConsumer::GetTrackedObjects() const
{
	TArray<UTrackedInstance*> trackedObjects;

	for (const TPair<FString, UTrackedInstance*>& pair : _instanceById)
	{
		if (pair.Value && pair.Value->Type == EInstanceType::Object)
		{
			trackedObjects.Add(pair.Value);
		}
	}

	return trackedObjects;
}

void UObjectDetectionConsumer::Connect(const FString& endpoint) {
	if (IsConnected()) {
		LOG("Error: ObjectDetection client cannot connect while is connected to %s.", *_endpoint);
		return;
	}
    GEngine->ForceGarbageCollection(true); //solves a unreal-crach-bug that is caused by an object with the same name on a system re-connect

	_isConnected = true;
	_endpoint = endpoint;
	_context = std::make_unique<grpc::ClientContext>();
	_stub = AR51::ObjectDetectionService::NewStub(grpc::CreateChannel(ToString(endpoint), grpc::InsecureChannelCredentials()));

	_consumerThread = std::thread([this]() { ConsumeObjectDetectionsLoop(); });
}

void UObjectDetectionConsumer::Disconnect() {
	if (!IsConnected()) {
		LOG("Error: ObjectDetection client cannot disconnect while not connected!");
		return;
	}
	_isConnected = false;
	if (_context) _context->TryCancel();
	if (_consumerThread.joinable()) _consumerThread.join();
	_context.reset();
	_stub.reset();
    GEngine->ForceGarbageCollection(true); //solves a unreal-crach-bug that is caused by an object with the same name on a system re-connect
}

void UObjectDetectionConsumer::InitRegistration() {
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

void UObjectDetectionConsumer::BeginPlay()
{
	Super::BeginPlay();

	LOG("Start playing...");
	if (!_isInitialized) {
		_isInitialized = true;
		LOG("Initializing...");
		InitRegistration();

        if (!MarkerMesh) LOG("Error: Marker mesh is null.");
        if (!MarkerMaterial) LOG("Error: Marker material is null.");
	}
}

void UObjectDetectionConsumer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    PruneInactive();
}

