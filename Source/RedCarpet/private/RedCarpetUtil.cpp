// Fill out your copyright notice in the Description page of Project Settings.


#include "RedCarpetUtil.h"

#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

ACameraActor* URedCarpetUtil::FindCameraInWorld(UWorld* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	for (TActorIterator<ACameraActor> It(WorldContextObject); It; ++It)
	{
		ACameraActor* retCamera = *It;
		return retCamera;
	}
	return nullptr;
}

FTransform URedCarpetUtil::GenerateRandomTransform(FVector center, float length, float width, float spawnHeight)
{
	FTransform retTransform;

	float randX = FMath::RandRange((center.X - length / 2), (center.X + length / 2));
	float randY = FMath::RandRange((center.Y - width / 2), (center.Y + width / 2));
	retTransform.SetLocation(FVector(randX, randY, spawnHeight));
	retTransform.SetRotation(FQuat(FRotator::ZeroRotator));
	return retTransform;
}
