#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceFactory.h"
#include "AR51LiveLinkSourceFactory.generated.h"

class SAR51LiveLinkSourceEditor;

/**
 * UAR51LiveLinkSourceFactory is a factory class used to create instances of LiveLink sources
 * specifically tailored for AR51 systems. 
 */
UCLASS()
class UAR51LiveLinkSourceFactory : public ULiveLinkSourceFactory
{
public:
	GENERATED_BODY()

#pragma region     === UI Methods =================================
	/**
	 * Provides the display name for this source factory. This name is used in the LiveLink UI
	 * where users select the type of LiveLink source to create.
	 *
	 * @return The user-friendly name of the LiveLink source type this factory creates.
	 */
	virtual FText GetSourceDisplayName() const;

	/**
	 * Provides a tooltip for this source factory, offering additional information about
	 * the kind of LiveLink source this factory is responsible for.
	 *
	 * @return A tooltip text that helps users understand what this factory does.
	 */
	virtual FText GetSourceTooltip() const;

	/**
	 * Defines how this factory should be represented in the LiveLink UI. In this case,
	 * it is represented as a standard menu entry.
	 *
	 * @return The type of menu entry, helping categorize this factory within the UI.
	 */
	virtual EMenuType GetMenuType() const;
#pragma endregion

	/**
	 * Creates a FAR51LiveLinkSource instance based on a given connection string. 
	 *
	 * @param ConnectionString A string containing all necessary parameters to configure the LiveLink source.
	 * @return A shared pointer to a newly created instance of a LiveLink source.
	 */
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& ConnectionString) const;	
};