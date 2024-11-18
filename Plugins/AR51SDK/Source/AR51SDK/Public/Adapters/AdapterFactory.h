#pragma once

#include "CoreMinimal.h"
#include "unreal.grpc.h"
#include "AR51UnrealSDK.h"
#include "Debug.h"
#include "PlatformType.h"

/// <summary>
/// The AdapterFactory class is a template class responsible for managing adapters of a specific platform type.
/// </summary>
/// <typeparam name="TAdapter">The type of the adapter that this factory produces.</typeparam>
template <typename TAdapter>
class AdapterFactory
{

    static EPlatformTypes ToEPlatformTypes(AR51::PlatformType type) { return (EPlatformTypes)(int)type; }
    static AR51::PlatformType ToPlatformType(EPlatformTypes type) { return (AR51::PlatformType)(int)type; }


protected:
    /// <summary>
    /// A map of adapters by platform type. 
    /// </summary>
    TMap<AR51::PlatformType, TAdapter*> _adaptersByPlatform;

protected:
    /// <summary>
    /// Provides access to the singleton instance of the AdapterFactory for a specific adapter type.
    /// </summary>
    /// <returns>The singleton instance of the AdapterFactory.</returns>
    static AdapterFactory<TAdapter>& Instance() {
        static AdapterFactory<TAdapter> instance;
        return instance;
    }

public:

    static void Clear() {
        Instance()._adaptersByPlatform = TMap<AR51::PlatformType, TAdapter*>();
    }

    /// <summary>
    /// Retrieves an adapter for the specified platform.
    /// </summary>
    /// <param name="platform">The platform for which an adapter is required.</param>
    /// <returns>The adapter for the specified platform, or null if no such adapter exists.</returns>
    static TAdapter* GetAdapter(AR51::PlatformType platform) {
        auto& instance = Instance();
        auto adapter = instance._adaptersByPlatform.Find(platform);
        return adapter ? *adapter : nullptr;
    }

    /// <summary>
    /// Retrieves an adapter for the specified platform.
    /// </summary>
    /// <param name="platform">The platform for which an adapter is required.</param>
    /// <returns>The adapter for the specified platform, or null if no such adapter exists.</returns>
    static TAdapter* GetAdapter(EPlatformTypes platform) {
        auto a = ToPlatformType(platform);
        return GetAdapter(a);
    }

    /// <summary>
    /// Retrieves an adapter for the platform provided by the AAR51SDK instance.
    /// </summary>
    /// <returns>The adapter for the platform specified by the AAR51SDK instance, or null if no such adapter exists.</returns>
    static TAdapter* GetAdapter() {
        if (AAR51SDK::Instance()) {
            return GetAdapter(AAR51SDK::Instance()->Platform);
        }
        else {
            LOG("Error: Could not find AR51SDK actor in level.");
            return nullptr;
        }
    }

    /// <summary>
    /// Registers a new adapter for the specified platform.
    /// </summary>
    /// <param name="platform">The platform for which the adapter should be registered.</param>
    /// <param name="adapter">The adapter to register for the specified platform.</param>
    static void Register(AR51::PlatformType platform, TAdapter* adapter)
    {
        if (!adapter)
        {
            LOG("Error: Attempt to register null adapter ignored.");
        }
        else if (Instance()._adaptersByPlatform.Contains(platform))
        {
            LOG("Error: Attempt to register an adapter for a platform that already has one. Overriding.");
            Instance()._adaptersByPlatform[platform] = adapter;
        }
        else {
            LOG("Registering Adapter: platform={%s}, type={%s}", *FString(PlatformType_Name(platform).c_str()), *FString("Adapter"));
            Instance()._adaptersByPlatform.Add(platform, adapter);
        }
    }
};