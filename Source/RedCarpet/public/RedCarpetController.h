// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarpetCharacter.h"
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

	virtual void Tick(float DeltaSeconds) override;
	
	TArray<ACarpetCharacter*> ListOfCharacters;

	UFUNCTION(BlueprintCallable)
	void GetAllCharacters();
	
protected:
	virtual void BeginPlay() override;

};
