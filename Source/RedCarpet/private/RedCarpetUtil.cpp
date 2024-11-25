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
