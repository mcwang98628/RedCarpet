﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetPickable.h"

#include "CarpetCharacter.h"
#include "RedCarpetBullet.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

FTimerHandle FireTimerHandle;

// Sets default values
ARedCarpetPickable::ARedCarpetPickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if(ItemMesh == nullptr)
	{
		ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	}
	SetRootComponent(ItemMesh);
	if(UBoxCollider == nullptr)
	{
		UBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	}
	UBoxCollider->SetupAttachment(RootComponent);
	UBoxCollider->SetGenerateOverlapEvents(true);
	UBoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // Enable collision queries
	UBoxCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // Set an appropriate collision channel
	UBoxCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // Respond to overlaps
}

// Called when the game starts or when spawned
void ARedCarpetPickable::BeginPlay()
{
	Super::BeginPlay();

	// UBoxCollider->OnComponentEndOverlap.AddDynamic(this, &ARedCarpetPickable::OnShapeOverlapEnd);
	
}

// Called every frame
void ARedCarpetPickable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsPickedUp)
	{
		endTime += DeltaTime;
		if(endTime - startTime >= ShootTimeInterval)
		{
			Shoot(FName("muzzle"));
			startTime = endTime;
		}
	}
}

void ARedCarpetPickable::Shoot(FName SpawnSocketName)
{
	// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AYourCharacter::FireBullet, 0.5f, true);
	if(Bullet)
	{
		// const USkeletalMeshSocket* muzzleSocket = ItemMesh->GetSocketByName(SpawnSocketName);;
		// FTransform socketTransform = muzzleSocket->GetSocketTransform(ItemMesh);
		const FVector MuzzleLocation = ItemMesh->GetSocketLocation(SpawnSocketName);
		// const FRotator MuzzleRotation = ItemMesh->GetSocketRotation(SpawnSocketName);

		const FRotator MuzzleRotation = GetActorRotation();


		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// Spawn the bullet at the muzzle
		ARedCarpetBullet* curBullet = GetWorld()->SpawnActor<ARedCarpetBullet>(Bullet, MuzzleLocation, MuzzleRotation, SpawnParams);
		
		// FVector newRot = MuzzleRotation.Rotator().Euler();
		curBullet->Direction = ItemMesh->GetRightVector();
	}
}