// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetEnemy.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ARedCarpetEnemy::ARedCarpetEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARedCarpetEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARedCarpetEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ACarpetCharacter* ARedCarpetEnemy::GetRandomCharacter()
{
	TArray<AActor*> retActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarpetCharacter::StaticClass(), retActor);
	if(retActor.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("ARedCarpetEnemy::GetRandomCharacter: No player in Scene"));
		return nullptr;
	}
	int32 ranIndex = FMath::RandRange(0, retActor.Num()- 1);
	ACarpetCharacter* retChar = Cast<ACarpetCharacter>(retActor[ranIndex]);
	return retChar;
}

