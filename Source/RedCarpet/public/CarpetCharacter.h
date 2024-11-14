// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCarpetPickable.h"
#include "GameFramework/Character.h"
#include "CarpetCharacter.generated.h"

UCLASS()
class REDCARPET_API ACarpetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACarpetCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UShapeComponent* UCharacterCollider;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnShapeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PickUpItem(ARedCarpetPickable* item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HeadMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ClothMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PantsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ShoesMesh;

private:
	void AdjustMaterialTiling(USkeletalMeshComponent* MeshComponent, USkeletalMesh* NewSkeletalMesh);
};
