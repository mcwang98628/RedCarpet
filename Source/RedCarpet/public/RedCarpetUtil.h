// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RedCarpetUtil.generated.h"

/**
 * 
 */
UCLASS()
class REDCARPET_API URedCarpetUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RedCarpetUtil | Camera", meta = (WorldContext="WorldContextObject"))
	static ACameraActor* FindCameraInWorld(UWorld* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "RedCarpetUtil | Transform")
	static FTransform GenerateRandomTransform(FVector center, float length, float width, float spawnHeight);
};
