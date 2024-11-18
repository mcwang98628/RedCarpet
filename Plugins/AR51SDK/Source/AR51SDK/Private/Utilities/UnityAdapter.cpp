#include "UnityAdapter.h"
#include "AR51UnrealSDK.h"

const float UnityAdapter::TO_UNREAL_UNIT_SCALE = 100.f;
const float UnityAdapter::TO_UNITY_UNIT_SCALE = 0.01f;

FLinearColor UnityAdapter::ParseColor(const FString& colorName) {
    static TMap<FString, FLinearColor> colorMap =
    {
        {TEXT("white"), FLinearColor::White},
        {TEXT("silver"), FLinearColor(0.75f, 0.75f, 0.75f)},
        {TEXT("gray"), FLinearColor::Gray},
        {TEXT("black"), FLinearColor::Black},
        {TEXT("red"), FLinearColor::Red},
        {TEXT("maroon"), FLinearColor(0.5f, 0.0f, 0.0f)},
        {TEXT("yellow"), FLinearColor::Yellow},
        {TEXT("olive"), FLinearColor(0.5f, 0.5f, 0.0f)},
        {TEXT("lime"), FLinearColor::Green},
        {TEXT("green"), FLinearColor(0.0f, 0.5f, 0.0f)},
        {TEXT("aqua"), FLinearColor(0.0f, 1.0f, 1.0f)},
        {TEXT("teal"), FLinearColor(0.0f, 0.5f, 0.5f)},
        {TEXT("blue"), FLinearColor::Blue},
        {TEXT("navy"), FLinearColor(0.0f, 0.0f, 0.5f)},
        {TEXT("fuchsia"), FLinearColor(1.0f, 0.0f, 1.0f)},
        {TEXT("purple"), FLinearColor(0.5f, 0.0f, 0.5f)},
        {TEXT("orange"), FLinearColor(1.0f, 0.647f, 0.0f)},
        {TEXT("pink"), FLinearColor(1.0f, 0.753f, 0.796f)},
        {TEXT("gold"), FLinearColor(1.0f, 0.843f, 0.0f)},
        {TEXT("beige"), FLinearColor(0.96f, 0.96f, 0.86f)},
        {TEXT("brown"), FLinearColor(0.65f, 0.16f, 0.16f)},
        {TEXT("coral"), FLinearColor(1.0f, 0.5f, 0.31f)},
        {TEXT("cyan"), FLinearColor(0.0f,1.0f,1.0f)},
        {TEXT("indigo"), FLinearColor(0.29f, 0.0f, 0.51f)},
        {TEXT("ivory"), FLinearColor(1.0f,1.0f, 0.94f)},
        {TEXT("khaki"), FLinearColor(0.94f, 0.9f, 0.55f)},
        {TEXT("lavender"), FLinearColor(0.9f, 0.9f, 0.98f)},
        {TEXT("lightblue"), FLinearColor(0.678f, 0.847f, 0.902f)},
        {TEXT("lightyellow"), FLinearColor(1.0f,1.0f, 0.878f)},
        {TEXT("magenta"), FLinearColor(1.0f, 0.0f,1.0f)},
        {TEXT("peach"), FLinearColor(1.0f, 0.855f, 0.725f)},
        {TEXT("plum"), FLinearColor(0.867f, 0.627f, 0.867f)},
        {TEXT("salmon"), FLinearColor(0.98f, 0.5f, 0.447f)},
        {TEXT("violet"), FLinearColor(0.933f, 0.51f, 0.933f)},
        {TEXT("tan"), FLinearColor(0.824f, 0.706f, 0.549f)},
        {TEXT("turquoise"), FLinearColor(0.251f, 0.878f, 0.816f)},
        {TEXT("yellowgreen"), FLinearColor(0.604f, 0.804f, 0.196f) }, 
        {TEXT("skyblue"), FLinearColor(0.53f, 0.808f, 0.922f)},
        {TEXT("crimson"), FLinearColor(0.863f, 0.078f, 0.235f)},
        {TEXT("darkblue"), FLinearColor(0.0f, 0.0f, 0.545f)},
        {TEXT("darkgreen"), FLinearColor(0.0f, 0.392f, 0.0f)},
        {TEXT("darkred"), FLinearColor(0.545f, 0.0f, 0.0f)},
        {TEXT("darkorange"), FLinearColor(1.0f, 0.549f, 0.0f)},
        {TEXT("darkviolet"), FLinearColor(0.58f, 0.0f, 0.827f)},
        {TEXT("lightgreen"), FLinearColor(0.565f, 0.933f, 0.565f)},
        {TEXT("lightpink"), FLinearColor(1.0f, 0.714f, 0.757f)},
        {TEXT("lightgray"), FLinearColor(0.827f, 0.827f, 0.827f)},
        {TEXT("lightgrey"), FLinearColor(0.827f, 0.827f, 0.827f)},
        {TEXT("darkgray"), FLinearColor(0.663f, 0.663f, 0.663f)},
        {TEXT("darkgrey"), FLinearColor(0.663f, 0.663f, 0.663f)},
        {TEXT("midnightblue"), FLinearColor(0.098f, 0.098f, 0.439f)},
        {TEXT("royalblue"), FLinearColor(0.255f, 0.412f, 0.882f)},
        {TEXT("forestgreen"), FLinearColor(0.133f, 0.545f, 0.133f)},
        {TEXT("slateblue"), FLinearColor(0.415f, 0.353f, 0.804f)},
        {TEXT("seagreen"), FLinearColor(0.18f, 0.545f, 0.341f)},
        {TEXT("darkslateblue"), FLinearColor(0.282f, 0.239f, 0.545f)},
        {TEXT("goldenrod"), FLinearColor(0.855f, 0.647f, 0.125f)},
        {TEXT("darkorchid"), FLinearColor(0.6f, 0.196f, 0.8f)},
        {TEXT("deepskyblue"), FLinearColor(0.0f, 0.749f,1.0f)},
        {TEXT("firebrick"), FLinearColor(0.698f, 0.133f, 0.133f)},
        {TEXT("cornflowerblue"), FLinearColor(0.392f, 0.584f, 0.929f)},
        {TEXT("darkgoldenrod"), FLinearColor(0.722f, 0.525f, 0.043f)},
        {TEXT("powderblue"), FLinearColor(0.69f, 0.878f, 0.902f)},
        {TEXT("rosybrown"), FLinearColor(0.737f, 0.561f, 0.561f)}
    };

    auto lowercaseColorName = colorName.ToLower();
    if (colorMap.Contains(lowercaseColorName))
    {
        return colorMap[lowercaseColorName];
    }
    else
    {
        return FLinearColor(0, 0, 0);
    }
}

/// <summary>
/// This method is used to get the main camera actor from the world.
/// </summary>
/// <returns>
/// The main camera actor in the world, or nullptr if not found.
/// </returns>
AActor* UnityAdapter::GetMainCamera()
{
    // Get the instance and world
    auto sdk = AAR51SDK::Instance();
    if (!sdk)
    {
        LOG("Error: AAR51SDK instance is not valid.");
        return nullptr;
    }

    auto world = sdk->GetWorld();
    if (!world)
    {
        LOG("Error: World is not valid.");
        return nullptr;
    }

    // Get the player controller and its camera manager
    auto playerController = world->GetFirstPlayerController();
    if (!playerController)
    {
        LOG("Error: PlayerController is not valid.");
        return nullptr;
    }

    auto playerCameraManager = playerController->PlayerCameraManager;
    if (!playerCameraManager)
    {
        LOG("Error: PlayerCameraManager is not valid.");
        return nullptr;
    }


    static TSoftObjectPtr<AActor> cameraActor = nullptr;
    static TSoftObjectPtr<UCameraComponent> cameraComponent = nullptr;

    bool isXR = GEngine && GEngine->XRSystem;


    // Get the target pawn
    APawn* viewTargetPawn = playerCameraManager->GetViewTargetPawn();
    if (viewTargetPawn)
    {
        cameraActor = viewTargetPawn;
        cameraComponent = GetComponentInChildren<UCameraComponent>(cameraActor.Get());
    }

    // Cache the camera actor
    if (!cameraActor || !cameraActor.IsValid() || !cameraComponent || !cameraComponent.IsValid())
    {
        // Search for the first camera; if in XR mode, search for the first camera that is locked to HMD
        for (auto a : GetAllActors(world))
        {
            auto c = GetComponentInChildren<UCameraComponent>(a);
            if (c && (!isXR || c->bLockToHmd))
            {
                cameraComponent = c;
                cameraActor = c->GetOwner();
                break;
            }
        }
    }

    // Force update camera position & orientation
    if (cameraComponent.IsValid() && cameraActor.IsValid())
    {
        FMinimalViewInfo f;

        cameraComponent->GetCameraView(1.f / 90.f, f);
        auto parent = cameraComponent->GetAttachParent();

        if (parent) parent->UpdateChildTransforms();
        else cameraComponent->UpdateChildTransforms();
    }
    else {
        cameraActor = nullptr;
        cameraComponent = nullptr;
    }

    return cameraActor ? cameraActor.Get() : nullptr;
}


TTransform UnityAdapter::GetMainCameraTransform() { return TTransform(GetComponent<UCameraComponent>(GetMainCamera())); };