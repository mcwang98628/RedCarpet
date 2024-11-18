#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"


#include "TrackedInstance.generated.h"

UENUM(BlueprintType)
enum class EInstanceType : uint8
{
	Marker UMETA(DisplayName = "Marker"),
	Object UMETA(DisplayName = "Object")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AR51SDK_API UTrackedInstance : public UActorComponent
{
	GENERATED_BODY()

public:

	UTrackedInstance() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	EInstanceType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	FString ColorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	FColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	TArray<FVector> Rays;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	float LastUpdateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instance")
	float Radius;

	FString ToString() const
	{
		return FString::Printf(TEXT("%s:%s@%s"), *UEnum::GetValueAsString(Type), *Id, *GetOwner()->GetActorLocation().ToString());
	}
};
