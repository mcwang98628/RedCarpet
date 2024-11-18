#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "NetworkUtils.generated.h"

/// <summary>
/// This class provides utility functions related to networking.
/// </summary>
UCLASS()
class AR51SDK_API UNetworkUtils : public UObject
{
    GENERATED_BODY()

public:
    /// <summary>
    /// Finds the local IP address.
    /// </summary>
    /// <returns>The local IP address as a string. Returns an empty string if the IP address is invalid or cannot be obtained.</returns>
    UFUNCTION(BlueprintPure, Category = "Networking", meta = (ToolTip = "Finds the local IP address. If successful, the IP address is returned as a string. If unsuccessful, an empty string is returned."))
    static FString GetLocalIp();

    /// <summary>
    /// Finds an available network port within the specified range.
    /// </summary>
    /// <param name="StartPort">The starting port number to check for availability.</param>
    /// <param name="OutPort">If successful, contains the number of the first available port found. If unsuccessful, contains -1.</param>
    /// <returns>True if an available port was found, false otherwise.</returns>
    UFUNCTION(BlueprintCallable, Category = "Networking", meta = (ToolTip = "Finds an available network port within the specified range. If successful, the first available port number is returned. If unsuccessful, -1 is returned."))
    static bool GetAvailablePort(int32 StartPort, int32& OutPort);

    /// <summary>
    /// Retrieves a list of local network addresses for the current device.
    /// </summary>
    /// <returns>
    /// An array of TSharedPtr<FInternetAddr> representing the local network addresses.
    /// If the socket subsystem cannot be accessed or no addresses are found, 
    /// the returned array will be empty.
    /// </returns> 
    static TArray<TSharedPtr<FInternetAddr>> GetLocalAddresses();
};
