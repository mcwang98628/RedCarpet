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

	ControlledPawn = GetPawn();

	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem -> AddMappingContext(DefaultMappingContext, 0);
	}
	// AToneCameraManager* cameraManager = Cast<AToneCameraManager>(PlayerCameraManager);
	// AToneCamera* mainCam = cameraManager->GetMainCameraActor();
	// cameraManager->SetMainCamera(mainCam, );
}

void ARedCarpetController::MoveFB(const FInputActionValue& val)
{
	FVector2D MovementInput = val.Get<FVector2D>();
	if (ControlledPawn)
	{
		// Calculate forward movement direction
		FVector ForwardDirection = ControlledPawn->GetActorRightVector();
		ControlledPawn->AddMovementInput(ForwardDirection, MovementInput.X);
	}
}

void ARedCarpetController::MoveLR(const FInputActionValue& val)
{
	FVector2D MovementInput = val.Get<FVector2D>();
	if (ControlledPawn)
	{
		// Calculate right movement direction
		FVector RightDirection = ControlledPawn->GetActorForwardVector();
		ControlledPawn->AddMovementInput(RightDirection, MovementInput.X);
	}
}

void ARedCarpetController::ChangeCloth(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Display, TEXT("Q Pressed"));
	GetAllCharacters();
	if(!ListOfCharacters.IsEmpty())
	{
		for(ACarpetCharacter* curChar: ListOfCharacters)
		{
			curChar->ChangeNextCloth();
		}
	}
}

void ARedCarpetController::ChangePants(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Display, TEXT("E Pressed"));
	GetAllCharacters();
	if(!ListOfCharacters.IsEmpty())
	{
		for(ACarpetCharacter* curChar: ListOfCharacters)
		{
			curChar->ChangeNextPants();
		}
	}
}

void ARedCarpetController::ToggleSunglasses(const FInputActionValue& val)
{
	UE_LOG(LogTemp, Display, TEXT("R Pressed"));
	GetAllCharacters();
	if(!ListOfCharacters.IsEmpty())
	{
		for(ACarpetCharacter* curChar: ListOfCharacters)
		{
			curChar->ToggleSunglasses();
		}
	}
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

void ARedCarpetController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveFrontBackAction, ETriggerEvent::Triggered, this, &ARedCarpetController::MoveFB);
		EnhancedInputComponent->BindAction(MoveLeftRightAction, ETriggerEvent::Triggered, this, &ARedCarpetController::MoveLR);
		EnhancedInputComponent->BindAction(ChangeClothAction, ETriggerEvent::Triggered, this, &ARedCarpetController::ChangeCloth);
		EnhancedInputComponent->BindAction(ChangePantsAction, ETriggerEvent::Triggered, this, &ARedCarpetController::ChangePants);
		EnhancedInputComponent->BindAction(ToggleSunglassesAction, ETriggerEvent::Triggered, this, &ARedCarpetController::ToggleSunglasses);
	}
}


