// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetBullet.h"


// Sets default values
ARedCarpetBullet::ARedCarpetBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARedCarpetBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARedCarpetBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector curLoc = GetActorLocation();
	FVector newLoc = curLoc + Direction * Speed * DeltaTime;
	SetActorLocation(newLoc);
	curLife += DeltaTime;
	if(curLife > LifeTime)
	{
		Destroy();
	}
	
}