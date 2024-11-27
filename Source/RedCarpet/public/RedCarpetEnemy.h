// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarpetCharacter.h"
#include "GameFramework/Actor.h"
#include "RedCarpetEnemy.generated.h"

UCLASS()
class REDCARPET_API ARedCarpetEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetEnemy();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EnemySpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ACarpetCharacter* TargetCharacter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	ACarpetCharacter* GetRandomCharacter();
};
