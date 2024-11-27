// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCarpetEnemy.h"
#include "GameFramework/Actor.h"
#include "RedCarpetEnemyManager.generated.h"

UCLASS()
class REDCARPET_API ARedCarpetEnemyManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARedCarpetEnemyManager();

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float SpawnInterval = 0;;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<ARedCarpetEnemy>> EnemyTypeList;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	int GetPlayerNumberInWorld();
	
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy(FTransform enemyTransform);

private:
	float startTime;
	float endTime;
};
