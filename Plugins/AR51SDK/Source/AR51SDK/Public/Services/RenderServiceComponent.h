#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"
#include "RenderServiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API URenderServiceComponent : public UActorComponent, public IServiceComponent<URenderServiceComponent>, public AR51::RenderService::Service
{
	GENERATED_BODY()

public:	
	URenderServiceComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
