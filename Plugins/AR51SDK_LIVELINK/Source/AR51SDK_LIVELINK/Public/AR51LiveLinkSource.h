#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkSource.h"
#include "Containers/Ticker.h"
#include "GameFramework/InputSettings.h"
#include "AR51UnrealSDK.h"
#include "EngineServiceComponent.h"
#include <unordered_map>
#include <AR51LiveLinkSourceSettings.h>

class ILiveLinkClient;

/**
 * Implements a live link source for AR51 SDK, integrating external tracking data into Unreal's LiveLink system.
 */
class AR51SDK_LIVELINK_API FAR51LiveLinkSource : public ILiveLinkSource
{
public:	

#pragma region === Constructor and Destructor ========================
	// Default constructor
	FAR51LiveLinkSource();
	// Destructor
	virtual ~FAR51LiveLinkSource();
#pragma endregion

#pragma region === ILiveLinkSource interface implementation ========================
	/**
	 * Called when the source is successfully added to the LiveLink client, providing a unique identifier.
	 * @param InClient Pointer to the ILiveLinkClient to which this source is added.
	 * @param InSourceGuid Unique identifier for this live link source.
	 */
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;

	/**
	 * Specifies the settings class to use for this live link source.
	 * @return Class type of the settings used by this live link source.
	 */
	virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override { return UAR51LiveLinkSourceSettings::StaticClass(); }

	/**
	 * Initializes settings for the live link source. Called after the source is added to the client.
	 * @param Settings Pointer to the settings object for this source.
	 */
	virtual void InitializeSettings(ULiveLinkSourceSettings* Settings) override;

	/**
	 * Requests the source to shut down, preparing for clean-up and removal.
	 * @return true if the source can be safely destroyed, false otherwise.
	 */
	virtual bool RequestSourceShutdown() override;


	/**
	 * Checks if the source is still valid and capable of updating live link data.
	 * @return true if the source is active and valid, false otherwise.
	 */
	virtual bool IsSourceStillValid() const override;

	/**
	 * Updates the source's state and processes new data. This function should execute quickly to not hinder performance.
	 */
	virtual void Update() override;


	/** Retrieves a human-readable identifier for the source type. */
	virtual FText GetSourceType() const override;

	/** Retrieves the machine name or location from which the source data originates. */
	virtual FText GetSourceMachineName() const override;

	/** Retrieves the current status of the live link source. */
	virtual FText GetSourceStatus() const override;
#pragma endregion

private:
	// Performs updates to the live link data, distributing it to the appropriate virtual subjects.
	void UpdateLiveLink();

#pragma region === Person managing methods ========================
	/** Adds a new tracking subject to the live link system if it represents a newly detected individual. */
	void PersonAdded(UAR51Character* Character);

	/** Automatically assigns tracking data to corresponding virtual subjects based on their availability. */
    void AutoAssignSubjects();

	/** Clears tracking data from virtual subjects starting from a specified index. */
    void ClearVirtualSubjects(int FromIndex = 0);

	/** Removes all tracking subjects from the system, typically used during cleanup. */
	void RemoveSourceSubjects();
#pragma endregion

#pragma region === Virtual Subject updating methods ========================
	/** Removes a specific subject's tracking data from the LiveLink client. */
	void RemoveSubject(FName SubjectName);

	/** Assigns a specific subject as the sole source for a virtual subject. */
	bool SetAutoAssignVirtualSubject(const FLiveLinkSubjectKey& Key, FName& SubjectName);

	/** Clears all tracking data associated with a specific virtual subject. */
	void ClearVirtualSubject(const FLiveLinkSubjectKey& Key);
#pragma endregion

	// Interface to the LiveLink client, used for sending and managing live link data.
	ILiveLinkClient* _liveLinkClient;

	// Unique identifier for this live link source within the LiveLink system.
	FGuid _liveLinkSourceGuid;

	// Custom settings instance for additional configuration specific to this live link source.
	UAR51LiveLinkSourceSettings* _settings = nullptr;

	// Set of subject names currently managed by this live link source.
	TSet<FName> _subjectNames;
};