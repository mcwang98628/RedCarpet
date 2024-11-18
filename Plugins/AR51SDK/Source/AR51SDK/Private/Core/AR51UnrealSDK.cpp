#include "AR51UnrealSDK.h"

#include "unreal.grpc.hpp"

#include "RegistrationClient.hpp"
#include "Debug.h"
#include "UnrealThread.h"
#include "SceneManagement.h"
#include <thread>
#include <EngineServiceComponent.h>
#include <GameServiceComponent.h>
#include <UnityAdapter.h>
#include <AnchorServiceComponent.h>
#include <RenderServiceComponent.h>
#include <HandSkeletonServiceComponent.h>
#include <DrawServiceComponent.h>
#include <CameraServiceComponent.h>
#include "NetworkUtils.h"
#include "HandsAdapterFactory.h"
#include "BoundaryAdapterFactory.h"


AAR51SDK::AAR51SDK()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
AAR51SDK::~AAR51SDK() = default;

void AAR51SDK::StartServices()
{
	LOG("SDK: Starting services...");


	_localIp = UNetworkUtils::GetLocalIp();
	_port = 0;

    auto startPort = ServicesPort > 0 ? ServicesPort : 4321;
    if (!UNetworkUtils::GetAvailablePort(startPort, _port)) {
        LOG("Error: could not find an available port. The system checked ports from 4321 to 65535 and all were in use. Ensure that fewer services or applications are using network ports, or expand the range of ports your application is allowed to use.");
    }
    else {
        if (ServicesPort > 0 && _port != ServicesPort) {
            LOG("Warning: The specified ServicesPort %d was not available. The system has assigned the closest available port %d.", ServicesPort, _port);
        }

        grpc::ServerBuilder builder;
        auto endpoint = ToString(_localIp) + ":" + std::to_string(_port);
        builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials(), &_port);


        // add EngineService, GameService, CameraService, RenderService, DrawService, AnchorService
        builder.RegisterService(UnityAdapter::GetComponent<UAnchorServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<UCameraServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<UDrawServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<UEngineServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<UGameServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<UHandSkeletonServiceComponent>(this));
        builder.RegisterService(UnityAdapter::GetComponent<URenderServiceComponent>(this));

        _server = builder.BuildAndStart();
        _endpoint = _localIp + ":" + FString::FromInt(_port);
        LOG("SDK: Services have started on %s", *_endpoint);
    }
}


void AAR51SDK::BeginPlay()
{
	UnrealThread::Start();

	StartServices();

	auto platform = (AR51::PlatformType)((int)Platform);
	_registrationClient = TSharedPtr<RegistrationClient>(new RegistrationClient(platform, IsDGS, _port, DiscoveryPort));
	_registrationClient->IsDiscoveryVerbose = &IsDiscoveryVerbose;
	_registrationClient->Start();

	Super::BeginPlay();
}


// Called every frame
void AAR51SDK::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UnrealThread::Tick();	
}

void AAR51SDK::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnrealThread::Stop();
	_server->Shutdown();

    BoundaryAdapterFactory::Clear();
    HandsAdapterFactory::Clear();
}

TSharedPtr<RegistrationClient> AAR51SDK::GetRegistration() { return _registrationClient; }

