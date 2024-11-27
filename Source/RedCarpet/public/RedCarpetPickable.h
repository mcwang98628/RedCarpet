// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedCarpetPickable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName = "Weapon"),
};
UCLASS()
class REDCARPET_API ARedCarpetPickable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetPickable();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Bullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootTimeInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPickedUp = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Shoot(FName SpawnSocketName);
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USkeletalMeshComponent* ItemMesh;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UShapeComponent* UBoxCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EItemType ItemType;

private:
	float startTime = 0;
	float endTime = 0;
};
