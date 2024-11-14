// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedCarpetPickable.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Cloth UMETA(DisplayName = "Cloth"),
	Head UMETA(DisplayName = "Head"),
	Pants UMETA(DisplayName = "Pants"),
	Shoes UMETA(DisplayName = "Shoes"),
	Weapon UMETA(DisplayName = "Weapon"),
};
UCLASS()
class REDCARPET_API ARedCarpetPickable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetPickable();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UShapeComponent* UBoxCollider;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EItemType ItemType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* ItemMesh;
};
