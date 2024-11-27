// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RedCarpetPickable.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "CarpetCharacter.generated.h"

UCLASS()
class REDCARPET_API ACarpetCharacter : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	ACarpetCharacter();
	
	UFUNCTION(BlueprintCallable)
	void SetupInputBindings();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* UCharacterCollider;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnShapeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HeadMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ClothMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* PantsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ShoesMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SunglassesMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMesh*>	ClothMeshList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMesh*>	PantsMeshList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ARedCarpetPickable>>	GunMeshList;
	
	UFUNCTION(BlueprintCallable)
	void RandomizeCloth();
	
	UFUNCTION(BlueprintCallable)
	void RandomizePants();

	UFUNCTION(BlueprintCallable)
	void ToggleSunglasses();

	UFUNCTION(BlueprintCallable)
	void GenerateRandomGun(FTransform spawnTransform);
	
	UFUNCTION(BlueprintCallable)
	void AttachStaticMeshToSocket(UMeshComponent* s_mesh, USceneComponent* parentComp, FName SocketName);

	UFUNCTION(BlueprintCallable)
	ARedCarpetPickable* FindUnusedGun();

	UFUNCTION(BlueprintCallable)
	bool HasAttachedGun();

	UFUNCTION(BlueprintCallable)
	void DestroyAttachedGun();
private:
	
	void AdjustMaterialTiling(USkeletalMeshComponent* MeshComponent, USkeletalMesh* NewSkeletalMesh);
	int32 curClothIndex = 0;

	int32 curPantsIndex = 0;

	bool IsSunglassesOn = true;

	// const FName noseBoneName = FName("SunglassesSocket");
};
