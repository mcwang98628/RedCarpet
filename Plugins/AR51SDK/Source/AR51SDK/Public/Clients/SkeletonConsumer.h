#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <vector>
#include <thread>


#include "unreal.grpc.h"
#include "AR51Character.h"
#include "ISingleton.h"
#include <Animation\SkeletalMeshActor.h>
#include <Animation\NodeMappingContainer.h>
#include "Materials/Material.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <fstream>
#include "SkeletonConsumer.generated.h"

class Person;
class AsyncHandSender;

UENUM(BlueprintType)
enum class EAutoScaleFilterType : uint8
{
	FT_Default UMETA(DisplayName = "Default"),
	FT_Kalman UMETA(DisplayName = "Kalman"),
	FT_MovingAverage UMETA(DisplayName = "Moving Average"),
	FT_MovingMedian UMETA(DisplayName = "Moving Median"),
	FT_SimpleSmoothing UMETA(DisplayName = "Simple Smoothing")
};

USTRUCT(BlueprintType)
struct FSolverParams {

	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Solver")
	bool Enabled{ true };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Solver")
	float CalfRatio{ 1.f };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Solver")
	bool EnableLimbPositioning{ true };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Scale")
	EAutoScaleFilterType FilterType { EAutoScaleFilterType::FT_SimpleSmoothing };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Scale")
	float SmoothingFactor{ 0.99 };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Scale")
	float MinValidScale{ 0.2 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Scale")
	float MaxValidScale{ 1.8 };

	bool IsValidScale(float scale);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AR51SDK_API USkeletonConsumer : public UActorComponent, public ISingleton<USkeletonConsumer>
{
	GENERATED_BODY()

public:
	// Determines if the model should be visible. True by default.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Determines if the model should be visible. True by default."))
	bool ShowModel{ true };

	// Determines if the skeleton should be visible. False by default.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool ShowSkeleton{ false };

	// Controllers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool ShowControllers{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	TArray<TSubclassOf<AActor>> CharacterBlueprints{};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool UpdateSkeleton{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool IsApplyHeadRotationWristPositionAndFingerRotations{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	float CalfRatio{ 0.95f };

	/// Controls the smoothing when falling back from the tracked to non-tracked state in the hand IK solver.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "Controls the smoothing when falling back from the tracked to non-tracked state in the hand IK solver."))
	float HandTrackingFallbackSmoothingFactor{ 0.99f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FSolverParams SolverParamters{ };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FString OverrideDeviceId{ "" };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UStaticMesh* ControllerMesh { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
		float ControllerSize{ 0.1f };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UStaticMesh* SkeletonMesh { nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	float SkeletonSize{ 0.025f };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Instantiation")
	float MaxCameraDistanceThreshold{ 20.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Instantiation")
	bool DestroyInactivePersons{ true };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Instantiation")
	float InactivePersonMaxSeconds{ 1.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Instantiation")
	float HidePersonMaxSeconds{ 0.2f };


	/// <summary>
	/// Indicates whether the system is currently in recording mode.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recording", meta = (ToolTip = "Indicates whether the system is currently in recording mode."))
	bool IsRecording{ false };

	/// <summary>
	/// Indicates whether the system is currently in playback mode.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recording", meta = (ToolTip = "Indicates whether the system is currently in playback mode."))
	bool IsPlayback{ false };

	/// <summary>
	/// Determines if the playback should loop once it reaches the end.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recording", meta = (ToolTip = "Determines if the playback should loop once it reaches the end."))
	bool IsPlaybackLoop{ false };

	/// <summary>
	/// Specifies the file path for saving and loading recordings.
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recording", meta = (ToolTip = "Specifies the file path for saving and loading recordings."))
	FString InOutRecordingFile { "/Recordings/file.txt" };
	
	// Event declaration using a dynamic multicast delegate without parameters
	/*DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkeletonUpdated)

	// BlueprintAssignable allows this event to be bound in Blueprints
	UPROPERTY(EditAnywhere, Category = "Skeleton Events")
	FOnSkeletonUpdated OnSkeletonUpdated;


	// BlueprintAssignable allows this event to be bound in Blueprints
	UPROPERTY(EditAnywhere, Category = "Skeleton Events")
	FOnSkeletonUpdated OnCharacterCreated;

	// Invoked before a character is being destoyed
	UPROPERTY(EditAnywhere, Category = "Skeleton Events")
	FOnSkeletonUpdated OnCharacterBeingDestroyed;*/


	// c'tor & D'tor
	USkeletonConsumer();
	~USkeletonConsumer();

private:

	bool _isInitialized{ false };
	bool _isConnected{ false };
	FString _endpoint{ "" };
	UInstancedStaticMeshComponent* _controllersIsm{ nullptr };
	UInstancedStaticMeshComponent* _skeletonIsm{ nullptr };

	std::thread _consumerThread;
	std::thread _playbackThread;
	std::unique_ptr<grpc::ClientContext> _context;
	std::unique_ptr<AR51::SkeletonService::Stub> _stub;


	std::shared_ptr<AsyncHandSender> _asyncHandSender;

	TMap < FString, std::shared_ptr<Person>> _personById;
	Person* _activePerson; 
	int32 _activeCharacterPrefabIndex;
	TSubclassOf<AActor> _activeCharacterBP;

	std::ofstream _recordingOutputFile;

private:

	std::shared_ptr<Person> CreatePerson(AR51::SkeletonReply& skeleton);
	void DestroyPerson(std::shared_ptr<Person>& person);

	int32 FindCharacterIndexByName(const FString& CharacterBPName);


	/// <summary>
	/// Removes a character from the _personById map and destroys its model.
	/// </summary>
	/// <param name="id">The unique identifier of the character to be reset.</param>
	void ResetCharacter(const FString& id);

	/// <summary>
	/// Destroys all the models of characters in the _personById map and then clears the map.
	/// </summary>
	void ResetCharacters();


	void OnSkeleton(AR51::SkeletonReply skeleton);

	void DrawControllers(const TArray<FVector>& positions);
	void DrawSkeleton(const TArray<FVector>& positions);

	void Visualize();
	void SolveIK();
	void SetHandsFromAdapter();
	void CopyHeadRotationFromXRCamera();
	Person* GetActivePerson();
	void SetActivePerson(Person* person);
	void UpdateActivePerson();

	void CheckInactivePersons();

	void ApplyHeadRotationWristPositionAndFingerRotations();
	void SendHeadRotationWristIKPosAndFingerRotationsToCV();


	void ConsumeSkeletonLoop();

	void Record(const AR51::SkeletonReply& skeleton);
	bool StartRecording();
	void StopRecording();

	void StartPlayback();
	void StopPlayback();

protected:
	void InitVisualization();
	void InitRegistration();
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	bool IsConnected();
	void Connect(const FString& endpoint);
	void Disconnect();


	/// <summary>
	/// Sets the active person's character using a character blueprint name.
	/// </summary>
	/// <param name="characterBPName">The name of the character blueprint to be set as the character to the active skeleton, i.e. the skeleton that the headset is associated with.</param>
	/// <returns>Returns true if successful, false otherwise.</returns>
	UFUNCTION(BlueprintCallable, Category = "Character", meta = 
		(ToolTip="Sets the active (i.e. the skeleton that the headset is associated with) character using the provided character blueprint name. Returns true if the operation is successful, false otherwise."))
	bool SetActiveCharacter(const FString& characterBPName);

	/// <summary>
	/// Gets a TArray of all tracked characters.
	/// </summary>
	TArray<UAR51Character*> GetCharacters() const;

	/// <summary>
	/// Gets the skeleton service endpoint info.
	/// </summary>
	/// <returns>Ip:port of the skeleton service endpoint</returns>
	FString GetEndpoint() const { return _endpoint; }
};
