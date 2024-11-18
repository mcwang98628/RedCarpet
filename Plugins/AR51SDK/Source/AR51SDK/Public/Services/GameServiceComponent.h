#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"
#include "GameServiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UGameServiceComponent : public UActorComponent, public IServiceComponent<UGameServiceComponent>, public AR51::GameService::Service
{
	GENERATED_BODY()

public:	
	UGameServiceComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
