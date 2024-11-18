// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkUtils.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
#include <SocketSubsystem.h>
#include "Interfaces/IPv4/IPv4SubnetMask.h"
#include <Debug.h>

FString UNetworkUtils::GetLocalIp() {
    bool canBind = false;
    auto localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
    auto sLocalIp = localIp->IsValid() ? localIp->ToString(false) : "";

#if defined(PLATFORM_WINDOWS) || defined(WITH_EDITOR)
    if (GetLocalAddresses().Num() > 1) {
        LOG("Warning: GetLocalIp returns the first valid ip: %s for %d network adapters on the PC. The local ip might be given on the wrong adapter", 
            *sLocalIp, GetLocalAddresses().Num());
    }
#endif

    return sLocalIp;
}

bool UNetworkUtils::GetAvailablePort(int32 StartPort, int32& OutPort)
{
    // Get the socket subsystem
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    // Initialize the output port to an invalid value
    OutPort = -1;

    // Iterate from the start port to the maximum allowable port
    for (int32 i = StartPort; i < 65535; ++i)
    {
        // Create an endpoint
        FIPv4Endpoint Endpoint(FIPv4Address::Any, i);
        TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();

        // Get the IPv4 address from the endpoint
        auto sRawIP = Endpoint.Address.ToString();
        bool bIsValid;
        Addr->SetIp(*sRawIP, bIsValid); // false indicates that the IP address is in host order (not network order)
        Addr->SetPort(i);

        // Attempt to bind the socket to the current port
        FSocket* ListenerSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("default"), false);
        if (ListenerSocket->Bind(*Addr))
        {
            // If the bind was successful, the port is available
            OutPort = i;

            // Clean up
            ListenerSocket->Close();
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);

            // Return true to indicate success
            return true;
        }
        else
        {
            // If the bind failed, close the socket and try the next port
            ListenerSocket->Close();
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
        }
    }

    // If no available port was found, return false
    return false;
}

TArray<TSharedPtr<FInternetAddr>> UNetworkUtils::GetLocalAddresses()
{
    TArray<TSharedPtr<FInternetAddr>> OutAddresses;

    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SocketSubsystem) return OutAddresses;

    if (!SocketSubsystem->GetLocalAdapterAddresses(OutAddresses))
    {
        LOG("Error: Failed in SocketSubsystem->GetLocalAdapterAddresses.");
    }

    return OutAddresses;
}
