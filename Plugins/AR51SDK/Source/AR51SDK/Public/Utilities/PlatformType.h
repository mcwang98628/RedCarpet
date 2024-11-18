#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM()
enum class EPlatformTypes : uint8
{
	PC = 0 UMETA(DisplayName = "PC"),
	HoloLens = 1 UMETA(DisplayName = "HoloLens"),
	HoloLens2 = 2 UMETA(DisplayName = "HoloLens2"),
	Android = 3 UMETA(DisplayName = "Android"),
	MagicLeap = 4 UMETA(DisplayName = "MagicLeap"),
	OculusRift = 5 UMETA(DisplayName = "OculusRift"),
	OculusQuest = 6 UMETA(DisplayName = "OculusQuest"),
	HtcVive = 7 UMETA(DisplayName = "HtcVive"),
	OpenXrTethered = 8 UMETA(DisplayName = "OpenXrTethered"),
	OpenXrMobile = 9 UMETA(DisplayName = "OpenXrMobile"),
	PicoNeo3Pro = 10 UMETA(DisplayName = "PicoNeo3Pro"),
	OvrTethered = 11 UMETA(DisplayName = "OvrTethered"),
	HtcFocus3 = 12 UMETA(DisplayName = "HtcFocus3"),
};
