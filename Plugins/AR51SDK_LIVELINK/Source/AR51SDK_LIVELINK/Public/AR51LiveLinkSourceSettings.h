#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceSettings.h"
#include "AR51LiveLinkSourceSettings.generated.h"

/**
 * Represents custom settings for the AR51 LiveLink source, allowing control over visual representation
 * of the associated AR51 'AR51Character' skeleton during runtime.
 * These settings are accessible and editable within the Unreal Engine editor's LiveLink plugin UI.
 */
UCLASS()
class AR51SDK_LIVELINK_API UAR51LiveLinkSourceSettings : public ULiveLinkSourceSettings
{
	GENERATED_BODY()

public:
    /**
     * Whether the AR51 character skeleton should be visible in the scene. This property can be toggled
     * from the LiveLink plugin UI in the Unreal Editor to control the visibility of the model
     * linked to the LiveLink data stream.
     */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool ShowModel{ false };
};
