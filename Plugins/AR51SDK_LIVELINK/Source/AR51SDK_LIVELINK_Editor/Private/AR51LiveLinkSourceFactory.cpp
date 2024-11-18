#include "AR51LiveLinkSourceFactory.h"
#include "AR51LiveLinkSource.h"
#include "ILiveLinkClient.h"
#include "LiveLinkSourceFactory.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "AR51LiveLinkSourceFactory"

#pragma region     === UI Methods =================================
FText UAR51LiveLinkSourceFactory::GetSourceDisplayName() const
{
	return LOCTEXT("SourceDisplayName", "AR51 LiveLink");
}
FText UAR51LiveLinkSourceFactory::GetSourceTooltip() const
{
	return LOCTEXT("SourceTooltip", "Creates a connection to the AR51 SDK to stream skeletal animations and object tracking.");
}
ULiveLinkSourceFactory::EMenuType UAR51LiveLinkSourceFactory::GetMenuType() const
{ 
	return EMenuType::MenuEntry; 
}
#pragma endregion

TSharedPtr<ILiveLinkSource> UAR51LiveLinkSourceFactory::CreateSource(const FString& ConnectionString) const
{
	auto source = MakeShared<FAR51LiveLinkSource>();
	return source;
}

#undef LOCTEXT_NAMESPACE