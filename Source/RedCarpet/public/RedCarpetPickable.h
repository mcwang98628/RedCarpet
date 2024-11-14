// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedCarpetPickable.generated.h"

UCLASS()
class REDCARPET_API ARedCarpetPickable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetPickable();

	enum class EItemType : uint8
	{
		Outfit UMETA(DisplayName = "Outfit"),
		Weapon UMETA(DisplayName = "Weapon"),
	};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UShapeComponent* UBoxCollider;


};
