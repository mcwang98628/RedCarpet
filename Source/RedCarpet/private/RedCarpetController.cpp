// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpet/Public/RedCarpetController.h"
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



void ARedCarpetController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARedCarpetController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ARedCarpetController::OnInputStarted);
		EnhancedInputComponent->BindAction(MoveFrontBackAction, ETriggerEvent::Triggered, this, &ARedCarpetController::MoveFB);
		EnhancedInputComponent->BindAction(MoveLeftRightAction, ETriggerEvent::Triggered, this, &ARedCarpetController::MoveLR);

		// EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ARedCarpetController::OnClickEnd);
		// EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ARedCarpetController::OnClickEnd);
	}
}


