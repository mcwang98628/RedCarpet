// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpet/Public/CarpetCharacter.h"

#include "RedCarpetPickable.h"
#include "Components/ShapeComponent.h"


// Sets default values
ACarpetCharacter::ACarpetCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UCharacterCollider = GetComponentByClass<UShapeComponent>();
}

// Called when the game starts or when spawned
void ACarpetCharacter::BeginPlay()
{
	Super::BeginPlay();
	UCharacterCollider->OnComponentBeginOverlap.AddDynamic(this, &ACarpetCharacter::OnShapeOverlapBegin);
}

// Called every frame
void ACarpetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACarpetCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACarpetCharacter::OnShapeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
										   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this)
	{
		ARedCarpetPickable* item = Cast<ARedCarpetPickable>(OtherActor);
		if (item)
		{
			// IsInActiveRange = true;
			// UE_LOG(LogTemp, Display, TEXT("Object In of active range: %s"), *GetName());
			PickUpItem(item);
		}
	}
}

void ACarpetCharacter::PickUpItem(ARedCarpetPickable* item)
{

	UE_LOG(LogTemp, Display, TEXT("Picked up object: %s"), *item->GetName());

	item->Destroy();
}

