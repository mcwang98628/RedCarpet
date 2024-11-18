#include "WorldAnchorConstraintComponent.h"
#include <AnchorServiceComponent.h>
#include <UnityAdapter.h>
#include "Debug.h"

// Sets default values for this component's properties
UWorldAnchorConstraintComponent::UWorldAnchorConstraintComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWorldAnchorConstraintComponent::BeginPlay()
{
	Super::BeginPlay();
	UnityAdapter::EnsureComponent<USceneComponent>(this);
	
}


// Called every frame
void UWorldAnchorConstraintComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UAnchorServiceComponent::Instance()) {
		LOG("Error: Anchor service is null.");
		return;
	}

	auto anchors = UAnchorServiceComponent::Instance()->GetAnchors();
	if (anchors.Num() > 0) {
		auto anchor = UnityAdapter::GetTransform(anchors[0]);
		auto transform = UnityAdapter::GetTransform(this);

		transform.SetPosition(anchor.GetPosition());
		transform.SetRotation(anchor.GetRotation());
	}
}

