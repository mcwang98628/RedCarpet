// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpet/Public/CarpetCharacter.h"

#include "RedCarpetPickable.h"
#include "Components/ShapeComponent.h"
#include "Engine/SkinnedAssetCommon.h"


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
	USkeletalMeshComponent* MainMeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	if (MainMeshComponent)
	{
		TArray<USkeletalMeshComponent*> SMComponents;
		TArray<USceneComponent*> ChildrenSC;
		MainMeshComponent->GetChildrenComponents(true, ChildrenSC);
		for (USceneComponent* Child : ChildrenSC)
		{
			USkeletalMeshComponent* SkeletalMeshChild = Cast<USkeletalMeshComponent>(Child);
			if (SkeletalMeshChild && SkeletalMeshChild->GetName() == TEXT("Pants"))
			{
				PantsMesh = SkeletalMeshChild;
				UE_LOG(LogTemp, Log, TEXT("Found Sub Mesh: %s"), *SkeletalMeshChild->GetName());
			}
			if (SkeletalMeshChild && SkeletalMeshChild->GetName() == TEXT("Clothes"))
			{
				ClothMesh = SkeletalMeshChild;
				UE_LOG(LogTemp, Log, TEXT("Found Sub Mesh: %s"), *SkeletalMeshChild->GetName());

			}
			if (SkeletalMeshChild && SkeletalMeshChild->GetName() == TEXT("Shoes"))
			{
				ShoesMesh = SkeletalMeshChild;
				UE_LOG(LogTemp, Log, TEXT("Found Sub Mesh: %s"), *SkeletalMeshChild->GetName());
			}

			UStaticMeshComponent* SMesh = Cast<UStaticMeshComponent>(Child);
			if(SMesh && SMesh->GetName() == TEXT("Sunglasses"))
			{
				SunglassesMesh = SMesh;
				UE_LOG(LogTemp, Log, TEXT("Found Sub Mesh: %s"), *SunglassesMesh->GetName());
				AttachSunglasses(MainMeshComponent);
			}
		}
	}
	
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
			// PickUpItem(item);
		}
	}
}

void ACarpetCharacter::PickUpItem(ARedCarpetPickable* item)
{

	UE_LOG(LogTemp, Display, TEXT("Picked up object: %s"), *item->GetName());

	if (item->ItemType == EItemType::Cloth)
	{
		USkeletalMeshComponent* newMesh = item->GetComponentByClass<USkeletalMeshComponent>();
		// ClothMesh->SetSkeletalMeshAsset(newMesh->GetSkeletalMeshAsset());
		ClothMesh->SetSkeletalMesh(newMesh->GetSkeletalMeshAsset());
		for (int32 i = 0; i < newMesh->GetMaterials().Num(); ++i)
		{
			if (newMesh->GetMaterials()[i])
			{
				ClothMesh->SetMaterial(i, newMesh->GetMaterials()[i]);
			}
		}
		AdjustMaterialTiling(ClothMesh, newMesh->GetSkeletalMeshAsset());
	}
	if (item->ItemType == EItemType::Shoes)
	{

		USkeletalMeshComponent* newMesh = item->GetComponentByClass<USkeletalMeshComponent>();
		ShoesMesh->SetSkeletalMeshAsset(newMesh->GetSkeletalMeshAsset());
		for (int32 i = 0; i < newMesh->GetMaterials().Num(); ++i)
		{
			if (newMesh->GetMaterials()[i])
			{
				ShoesMesh->SetMaterial(i, newMesh->GetMaterials()[i]);
			}
		}
		AdjustMaterialTiling(ShoesMesh, newMesh->GetSkeletalMeshAsset());
	}
	if (item->ItemType == EItemType::Pants)
	{
		USkeletalMeshComponent* newMesh = item->GetComponentByClass<USkeletalMeshComponent>();
		PantsMesh->SetSkeletalMeshAsset(newMesh->GetSkeletalMeshAsset());

		for (int32 i = 0; i < newMesh->GetMaterials().Num(); ++i)
		{
			if (newMesh->GetMaterials()[i])
			{
				PantsMesh->SetMaterial(i, newMesh->GetMaterials()[i]);
			}
		}
		AdjustMaterialTiling(PantsMesh, newMesh->GetSkeletalMeshAsset());
	}
	
	item->Destroy();
}

void ACarpetCharacter::ChangeNextCloth()
{
	curClothIndex++;
	curClothIndex = curClothIndex % ClothMeshList.Num();
	
	USkeletalMesh* newMesh = ClothMeshList[curClothIndex];
	ClothMesh->SetSkeletalMesh(newMesh);
	
	for (int32 i = 0; i < newMesh->GetMaterials().Num(); ++i)
	{
		UMaterialInterface* Material = newMesh->GetMaterials()[i].MaterialInterface;
		if (Material)
		{
			ClothMesh->SetMaterial(i, Material);
		}
		
	}
	// AdjustMaterialTiling(ClothMesh, newMesh);
}

void ACarpetCharacter::ChangeNextPants()
{
	curPantsIndex++;
	curPantsIndex = curPantsIndex % PantsMeshList.Num();
	
	USkeletalMesh* newMesh = PantsMeshList[curPantsIndex];
	PantsMesh->SetSkeletalMesh(newMesh);
	
	for (int32 i = 0; i < newMesh->GetMaterials().Num(); ++i)
	{
		UMaterialInterface* Material = newMesh->GetMaterials()[i].MaterialInterface;
		if (Material)
		{
			PantsMesh->SetMaterial(i, Material);
		}
		
	}
}

void ACarpetCharacter::ToggleSunglasses()
{
	IsSunglassesOn = !IsSunglassesOn;
	SunglassesMesh->SetVisibility(IsSunglassesOn);
}

void ACarpetCharacter::AdjustMaterialTiling(USkeletalMeshComponent* MeshComponent, USkeletalMesh* NewSkeletalMesh)
{
	if (!MeshComponent || !NewSkeletalMesh)
	{
		return;
	}

	// Get the bounds of the new mesh to determine the scaling
	FVector MeshBounds = NewSkeletalMesh->GetBounds().BoxExtent;

	// Calculate tiling scale based on mesh dimensions (adjust as needed)
	float ScaleU = 1.0f / FMath::Max(MeshBounds.X, 1.0f); // Prevent division by zero
	float ScaleV = 1.0f / FMath::Max(MeshBounds.Y, 1.0f);

	// Get the dynamic material instance for the first material slot
	UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("TextureScaleU"), ScaleU);
		DynamicMaterial->SetScalarParameterValue(FName("TextureScaleV"), ScaleV);
	}

	// Repeat for other material slots if needed
	for (int32 i = 1; i < MeshComponent->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(i);
		if (DynamicMaterialInstance)
		{
			DynamicMaterialInstance->SetScalarParameterValue(FName("TextureScaleU"), ScaleU);
			DynamicMaterialInstance->SetScalarParameterValue(FName("TextureScaleV"), ScaleV);
		}
	}
}

void ACarpetCharacter::AttachSunglasses(USceneComponent* parentComponent)
{
	SunglassesMesh->AttachToComponent(parentComponent, FAttachmentTransformRules::SnapToTargetIncludingScale,noseBoneName);
}

