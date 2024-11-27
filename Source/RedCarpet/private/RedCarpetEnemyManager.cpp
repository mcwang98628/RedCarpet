// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetEnemyManager.h"

#include "CarpetCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARedCarpetEnemyManager::ARedCarpetEnemyManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARedCarpetEnemyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARedCarpetEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// endTime += DeltaTime;
	// if(endTime - startTime >= SpawnInterval)
	// {
	// 	// Shoot(FName("muzzle"));
	// 	startTime = endTime;
	// }
}

int ARedCarpetEnemyManager::GetPlayerNumberInWorld()
{
	TArray<AActor*> retActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarpetCharacter::StaticClass(), retActor);

	return retActor.Num();
}

void ARedCarpetEnemyManager::SpawnEnemy(FTransform enemyTransform)
{
	if(EnemyTypeList.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy List Empty"));
		return;
	}
	UWorld* curWorld = GetWorld();

	int32 ranNumInRange = FMath::RandRange(0, EnemyTypeList.Num()) % EnemyTypeList.Num();
	TSubclassOf<ARedCarpetEnemy> SelectedClass = EnemyTypeList[ranNumInRange];
	
	if(curWorld)
	{
		curWorld->SpawnActor<ARedCarpetEnemy>(SelectedClass, enemyTransform);
	}
}

