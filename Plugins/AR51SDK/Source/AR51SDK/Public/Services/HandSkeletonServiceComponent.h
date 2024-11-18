#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"
#include "HandSkeletonServiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UHandSkeletonServiceComponent : public UActorComponent, public IServiceComponent<UHandSkeletonServiceComponent>, public AR51::HandSkeletonService::Service
{
	GENERATED_BODY()

public:	
	UHandSkeletonServiceComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual ::grpc::Status StartStreaming(::grpc::ServerContext* context, const ::AR51::Empty* request,
		::grpc::ServerWriter< ::AR51::TwoHandsInfoResponse>* writer) override;

};
