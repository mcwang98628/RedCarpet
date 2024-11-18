// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "SkeletonConsumer.h"
#include <thread>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "ISingleton.h"
#include "unreal.grpc.h"
#include "PlatformType.h"

#include "AR51UnrealSDK.generated.h"

class RegistrationClient;

UCLASS()
class AR51SDK_API AAR51SDK : public AActor, public ISingleton<AAR51SDK>
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FString Version {"1.0.0.0"};

	/// <summary>
	/// The Port for the discovery service.
	/// </summary>
	/// <remarks>
	/// This property specifies the port where the discovery service will be hosted.
	/// </remarks>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "The Port where the discovery service will be hosted."))
	int DiscoveryPort{ 1500 };
	
	/// <summary>
	/// The Port for the device services.
	/// </summary>
	/// <remarks>
	/// This property specifies the port where the device services will be hosted. 
	/// If the value is -1, a random available port will be chosen. 
	/// If a specific port is provided and it is not available, the system will attempt to find the closest available port.
	/// </remarks>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General", meta = (ToolTip = "The Port where the device services will be hosted. If the value is -1, a random available port will be chosen. If a specific port is provided and it is not available, the system will attempt to find the closest available port."))
	int ServicesPort{ -1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool IsDiscoveryVerbose{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool IsDGS{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	EPlatformTypes Platform{ EPlatformTypes::PC };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool IsOnScreenDebugMessage{ false };

private:
	FString _localIp;
	int32 _port{ 0 };
	FString _endpoint;
	FSocket* _socket;

	std::unique_ptr<grpc::Server> _server;
	TSharedPtr<RegistrationClient> _registrationClient;
	USkeletonConsumer* _skeletonConsumer;

private:

	void StartServices();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	AAR51SDK();
	~AAR51SDK();

	virtual void Tick(float DeltaTime) override;

	TSharedPtr<RegistrationClient> GetRegistration();
};
