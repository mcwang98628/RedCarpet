// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "RedCarpetController.generated.h"

/**
 * 
 */
UCLASS()
class REDCARPET_API ARedCarpetController : public APlayerController
{
	GENERATED_BODY()


public:
	ARedCarpetController();

	APawn* ControlledPawn;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveFrontBackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveLeftRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeClothAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangePantsAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ToggleSunglassesAction;

	void MoveFB(const FInputActionValue& val);
	void MoveLR(const FInputActionValue& val);
	void ChangeCloth(const FInputActionValue& val);
	void ChangePants(const FInputActionValue& val);
	void ToggleSunglasses(const FInputActionValue& val);
	
protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

};
