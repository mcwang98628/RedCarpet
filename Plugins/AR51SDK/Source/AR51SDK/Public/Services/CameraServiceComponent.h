#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"
#include "CameraServiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UCameraServiceComponent : public UActorComponent, public IServiceComponent<UCameraServiceComponent>, public AR51::CameraService::Service
{
	GENERATED_BODY()

public:	
	UCameraServiceComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
