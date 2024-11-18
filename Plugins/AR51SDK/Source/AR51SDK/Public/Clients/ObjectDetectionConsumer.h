#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <vector>
#include <thread>

#include "unreal.grpc.h"
#include "ISingleton.h"
#include "TrackedInstance.h"

#include "ObjectDetectionConsumer.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AR51SDK_API UObjectDetectionConsumer : public UActorComponent, public ISingleton<UObjectDetectionConsumer>
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "General")
	UMaterial* MarkerMaterial;

	UPROPERTY(EditAnywhere, Category = "General")
	UStaticMesh* MarkerMesh;

	UPROPERTY(EditAnywhere, Category = "General")
	UStaticMesh* DefaultObjectMesh;

	UPROPERTY(EditAnywhere, Category = "General")
	bool ShowMarkers{ true };


	UPROPERTY(EditAnywhere, Category = "General")
	TArray<TSubclassOf<AActor>> Prefabs;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smoothing")
	float SmoothRotationFactor = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smoothing")
	float SmoothPositionalFactor = 0.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smoothing")
	float SmoothRotationAngleThreshold = 12.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Smoothing")
	float SmoothPositionalThreshold = 0.03f;

public:

	// c'tor & D'tor
	UObjectDetectionConsumer();
	~UObjectDetectionConsumer();


public:
	bool IsConnected();

	TArray<UTrackedInstance*> GetTrackedMarkers() const;

	TArray<UTrackedInstance*> GetTrackedObjects() const;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	bool _isInitialized{ false };
	bool _isConnected{ false };
	FString _endpoint{ "" };
	std::thread _consumerThread;
	std::unique_ptr<grpc::ClientContext> _context;
	std::unique_ptr<AR51::ObjectDetectionService::Stub> _stub;


	TMap<FString, UTrackedInstance*> _instanceById;

	void PruneInactive();
	void OnObjectDetected(AR51::ObjectDetectionReply e);
	void ConsumeObjectDetectionsLoop();

	void Connect(const FString& endpoint);
	void Disconnect();
	void InitRegistration();
};
