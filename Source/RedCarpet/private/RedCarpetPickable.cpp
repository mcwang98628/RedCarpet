// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetPickable.h"

#include "CarpetCharacter.h"
#include "Components/BoxComponent.h"


// Sets default values
ARedCarpetPickable::ARedCarpetPickable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
}

