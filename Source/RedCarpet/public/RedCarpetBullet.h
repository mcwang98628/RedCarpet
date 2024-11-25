// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RedCarpetBullet.generated.h"

UCLASS()
class REDCARPET_API ARedCarpetBullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetBullet();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Direction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float curLife = 0;
};
