#pragma once

#include "CoreMinimal.h"
#include "LiveLinkAnimationVirtualSubject.h"
#include "AR51LiveLinkSubject.generated.h"

/**
 * UAR51LiveLinkSubject is a custom implementation of a virtual LiveLink subject that
 * is tailored to handle specific data associated with tracked people from the AR51 SDK.
 * This class allows dynamic control over the subjects it handles, including
 * adding and clearing linked subjects.
 */
UCLASS(Blueprintable, Category = "LiveLink", meta = (DisplayName = "AR51 LiveLink"))
class AR51SDK_LIVELINK_API UAR51LiveLinkSubject: public ULiveLinkAnimationVirtualSubject
{
	GENERATED_BODY()
	
public:
	/**
	 * Sets the subject to be tracked by this virtual subject. This function updates the virtual
	 * subject to only follow the specified subject name, effectively focusing the LiveLink
	 * data flow to a particular target, identified by `subjectName`.
	 *
	 * @param subjectName The name of the LiveLink subject to be tracked.
	 */
	void SetSubject(FName SubjectName);

	/**
	 * Clears all currently tracked subjects from this virtual subject. 
	 */
	void ClearSubjects();
};
