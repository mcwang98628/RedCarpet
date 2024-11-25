// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpet/Public/RedCarpetController.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


ARedCarpetController::ARedCarpetController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ARedCarpetController::BeginPlay()
{
	Super::BeginPlay();

	// ControlledPawn = GetPawn();
	
	// AToneCameraManager* cameraManager = Cast<AToneCameraManager>(PlayerCameraManager);
	// AToneCamera* mainCam = cameraManager->GetMainCameraActor();
	// cameraManager->SetMainCamera(mainCam, );
}

void ARedCarpetController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARedCarpetController::GetAllCharacters()
{
	ListOfCharacters.Empty();
	UWorld* currentWorld = GetWorld();
	for (TActorIterator<ACarpetCharacter> It(currentWorld->GetWorld()); It; ++It)
	{
		ACarpetCharacter* aCharacter = Cast<ACarpetCharacter>(*It);
		if(aCharacter)
		{
			ListOfCharacters.Add(aCharacter);
		}
	}
}