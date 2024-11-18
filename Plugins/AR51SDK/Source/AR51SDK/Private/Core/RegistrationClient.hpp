#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Networking.h"

#include <thread>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <functional>

#include "UEvent.h"
#include "Debug.h"
#include "Converter.h"
#include "Utils.h"
#include "NetworkUtils.h"

#include "unreal.grpc.h"

class RegistrationClient {
public:
	UEvent<void*, const AR51::ComponentDescriptor&> OnComponentAdded;
	UEvent<void*, const AR51::ComponentDescriptor&> OnComponentRemoved;

private:
	bool _isRunning{ false };
	bool _isConnected{ false };
	FString _deviceId{ "" };
	FSocket* _socket{ nullptr };
	FString _omsAddress;

	std::thread _reconnectThread;
	std::thread _registrationThread;

	std::thread _keepAliveThread;
	std::unique_ptr< grpc::ClientContext> _keepAliveContext;

	std::thread _listenToComponentsThread;
	std::unique_ptr<grpc::ClientContext> _listenToComponentsContext;

	std::vector<AR51::ComponentDescriptor> _components;

public:
	const std::string Version = "1.0.0";

	const AR51::PlatformType Platform;
	const bool IsDGS;
	const FString LocalIp;
	const int Port;
	const int DiscoveryPort;

	bool* IsDiscoveryVerbose{ nullptr };
private:

	bool IsDiscoveryVerboseCore() const { return IsDiscoveryVerbose && *IsDiscoveryVerbose; }

	FString DiscoverOnce(int timeoutInMs) {
		FString ip;
		if(IsDiscoveryVerboseCore()) LOG("Discovering server on port: %d started...", DiscoveryPort);

		const int bufferSize = 1024;

		if (_socket == nullptr) {
			_socket = FUdpSocketBuilder(*FString("Discovery"))
				.AsNonBlocking()
				.AsReusable()
				.WithReceiveBufferSize(bufferSize)
				.WithBroadcast()
				.WithSendBufferSize(bufferSize).Build();
		}

		auto broadcastAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		auto remoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		broadcastAddress->SetBroadcastAddress();
		broadcastAddress->SetPort(DiscoveryPort);

		int32 byteSent = 0;

		auto waitTime = FTimespan::FromMilliseconds(timeoutInMs);

		if (IsDiscoveryVerboseCore()) LOG("Broadcasting version: \"%s\" to the server...", *FString(Version.c_str()));
		if (_socket->SendTo((uint8*)Version.c_str(), Version.size(), byteSent, *broadcastAddress)) {

			if (IsDiscoveryVerboseCore()) LOG("Discovery server received %d bytes.", byteSent);
			char buffer[bufferSize];
			std::memset(buffer, 0, bufferSize);

			if (_socket->Wait(ESocketWaitConditions::WaitForRead, waitTime) &&
				_socket->RecvFrom((uint8*)buffer, bufferSize, byteSent, *remoteAddress)) {

				FString port(buffer);
				ip = remoteAddress->ToString(false) + ":" + port;
				if (IsDiscoveryVerboseCore()) LOG("Discovery server sent %d bytes from ip: %s and registration port: %s",
					byteSent, *remoteAddress->ToString(true), *port);
			}
			else {
				if (IsDiscoveryVerboseCore()) LOG("FAILED receiving bytes from discovery server. Receiving only %d bytes.", byteSent);
			}
		}
		else {
			if (IsDiscoveryVerboseCore()) LOG("FAILED sending bytes to discovery server: %d bytes...", byteSent);
		}
		return ip;
	}

	FString Discover(bool& isRunning, int timeoutInMs = 250, int retryInteralInMs = 1000, int retryCount = 10000)
	{
		FString ip;

		for (auto i = 0; i < retryCount && isRunning; i++)
		{
			ip = DiscoverOnce(timeoutInMs);
			if (!ip.IsEmpty()) break;
			std::this_thread::sleep_for(std::chrono::milliseconds(retryInteralInMs));
		}

		if (!timeoutInMs && IsDiscoveryVerboseCore()) LOG("Discovery was cancelled!");

		return ip;
	}

	AR51::ComponentList GetComponent() {

		AR51::ComponentList response;
		auto id = ToString(GetDeviceId());
		auto name = ToString(FPlatformProcess::ComputerName());
		auto mac = ToString(FGenericPlatformMisc::GetMacAddressString());
		auto localIp = ToString(LocalIp);
		auto port = Port;
		auto type = IsDGS ? AR51::ComponentType::DGSComponent : AR51::ComponentType::DeviceComponent;

		// set the component 
		auto c = response.add_components();
		c->set_id(id);
		c->set_name(name);
		c->set_ip(localIp);
		c->set_port(port);
		c->set_macaddress(mac);
		c->set_type(type);
		c->set_platform(Platform);

		c->add_services("GameService");
		c->add_services("UnityService");
		c->add_services("DrawService");
		c->add_services("AnchorService");
		c->add_services("CameraService");
		c->add_services("RenderService");

		return response;
	}

	void KeepALiveLoop() {
		//try
		{
			LOG("Registration: Keep alive loop started...");

			auto omsEndpoint = ToString(_omsAddress);
			AR51::IsAliveQuery query;
			AR51::ComponentList response = GetComponent();

			// Connecting to Registration services.
			auto registrationChannel = grpc::CreateChannel(omsEndpoint, grpc::InsecureChannelCredentials());
			auto registrationServiceStub = AR51::RegistrationService::NewStub(registrationChannel);
			
			std::unique_ptr< grpc::ClientReaderWriter<AR51::ComponentList, AR51::IsAliveQuery>> duplex(registrationServiceStub->Register(_keepAliveContext.get()));

			do {
				duplex->Write(response);
			} while (duplex->Read(&query));

			auto status = duplex->Finish();

			if (status.ok()) {
				LOG("Registration: Keep alive loop completed successfully.");
			}
			else if (status.error_code() == grpc::StatusCode::CANCELLED) {
				LOG("Registration: Keep alive loop completed with Warning: %s.", *FString(status.error_message().c_str()));
			}
			else {
				LOG("Registration: Keep alive loop completed with Error: %s.", *FString(status.error_message().c_str()));
			}
		}
		//catch (const std::exception& ex)
		//{
		//	LOG("Registration: Keep alive loop failed. Error: %s", *ToFString(ex.what()));
		//}
		_isConnected = false;
	}

	void ListenToComponentLoop() {
		//try
		{
			LOG("Registration: Listen to components loop started...");

			auto omsEndpoint = ToString(_omsAddress);

			// Connecting to Registration services.
			auto registrationChannel = grpc::CreateChannel(omsEndpoint, grpc::InsecureChannelCredentials());
			auto registrationServiceStub = AR51::RegistrationService::NewStub(registrationChannel);

			AR51::Empty request;
			AR51::ComponentsChanged response;
			auto stream = registrationServiceStub->ListenToComponents(_listenToComponentsContext.get(), request);

			while (stream->Read(&response))
			{
				switch (response.type())
				{
				case AR51::ComponentChangedType::ComponentCurrentState:
				case AR51::ComponentChangedType::ComponentAdded:
					for (auto& component : response.components()) {
						LOG("Registration: Component added: %s at %s@%d", *FString(component.name().c_str()), *FString(component.ip().c_str()), component.port());

						_components.push_back(component);
						OnComponentAdded.Invoke(this, component);
					}
					break;
				case AR51::ComponentChangedType::ComponentRemoved:
					for (auto& component : response.components()) {
						LOG("Registration: Component removed: %s at %s@%d", *FString(component.name().c_str()), *FString(component.ip().c_str()), component.port());
						auto it = std::find_if(_components.begin(), _components.end(), [&component](const AR51::ComponentDescriptor& c) { return c.id() == component.id(); });
						if(it != _components.end()) _components.erase(it);
						OnComponentRemoved.Invoke(this, component);
					}
					break;
				default:
					break;
				}
			}

			auto status = stream->Finish();
			if (status.ok())
				LOG("Registration: ListenToComponentLoop completed successfully.");
			else if (status.error_code() == grpc::StatusCode::CANCELLED)
				LOG("Registration: Failed in ListenToComponentLoop. Warning: %s", *ToFString(status.error_message()));
			else
				LOG("Registration: Failed in ListenToComponentLoop. Error: %s", *ToFString(status.error_message()));
		}
		//catch (const std::exception& ex)
		//{
		//	LOG("Registration: Listen To Component Loop failed. Error: %s", *ToFString(ex.what()));
		//}
		//catch (...) {
		//	LOG("Registration: Listen To Component Loop failed. Error: Unknown.");
		//}
	}

	void ReconnectLoop() {

		LOG("Starting reconnection loop...");
		Connect();
		while (_isRunning)
		{
			if (!_isConnected)
			{
				LOG("Disconnected from OMS, trying to reconnect...");
				Connect();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		LOG("Reconnection loop successfully completed.");
	}

	void Connect() {
		if (_keepAliveContext != nullptr) Disconnect();

		_isConnected = true;

		// Discovering for OMS
		LOG("Registration: Searching for OMS on discovery port: %d", DiscoveryPort);
		_omsAddress = Discover(_isRunning);

		if (_omsAddress.IsEmpty()) {
			_isConnected = false;
			return;
		}
		LOG("Registration: OMS found on: %s", *_omsAddress);

		// Start keep a live loop	
		_keepAliveContext = std::make_unique<grpc::ClientContext>();
		_keepAliveThread = std::thread([this]() { KeepALiveLoop(); });

		// Start listening to components to feed devices to the device manager
		_listenToComponentsContext = std::make_unique<grpc::ClientContext>();
		_listenToComponentsThread = std::thread([this]() { ListenToComponentLoop(); });
	}

	void Disconnect() {
		LOG("Registration: Disconnecteding from OMS.");


		LOG("Registration: Killing keep alive thread...");

		if (_keepAliveContext != nullptr) {
			_keepAliveContext->TryCancel();
		}

		if (_keepAliveThread.joinable()) {
			_keepAliveThread.join();
			_keepAliveContext.reset();
		}



		LOG("Registration: Killing listen to components thread...");
		if (_listenToComponentsContext != nullptr) {
			_listenToComponentsContext->TryCancel();
		}
		if (_listenToComponentsThread.joinable()) {
			_listenToComponentsThread.join();
			_listenToComponentsContext.reset();
		}

		_isConnected = false;
		LOG("Registration: Successfully disconnected from OMS.");
	}

public:
	bool IsRunning() const { return _isRunning; }
	bool IsConnected() const { return _isConnected; }

	void Start() {
		if (_isRunning) {
			LOG("Registration: Error: Cannot start while running!");
			return;
		}
		_isRunning = true;
		_reconnectThread = std::thread([this]() { ReconnectLoop(); });
	}

	void Stop() {
		if (!_isRunning) {
			LOG("Registration: Error: Cannot stop registration client, it is NOT running!");
			return;
		}

		LOG("Registration: Stopping registration client...");

		_isRunning = false;
		if (_reconnectThread.joinable()) {

			LOG("Registration: Stopping registration reconnection loop...");
			_reconnectThread.join();

			LOG("Registration: Reconnection registration stopped successfully.");
		}
		Disconnect();

		LOG("Registration client stopped successfully!");

	}

	/// <summary>
	/// Attempts to retrieve a unique device identifier using various platform-specific methods.
	/// The methods are tried in the following order: GetDeviceId, GetUniqueDeviceId,
	/// GetMachineId, GetHashedMacAddressString, and GetOperatingSystemId.
	/// </summary>
	/// <returns>
	/// A non-empty string containing the device identifier if successful; otherwise, an empty string.
	/// Logs a warning using the LOG macro for each method that fails to return a valid ID.
	/// </returns>
	const FString& GetDeviceId()
	{

		// Check if DeviceId has already been initialized
		if (!_deviceId.IsEmpty())
		{
			return _deviceId;
		}

		// Try FPlatformMisc::GetDeviceId()
		_deviceId = FPlatformMisc::GetDeviceId();
		if (!_deviceId.IsEmpty())
			return _deviceId;
		LOG("Warning: FPlatformMisc::GetDeviceId() returned an empty string.");

		// Try FPlatformMisc::GetUniqueAdvertisingId()
		_deviceId = FPlatformMisc::GetUniqueAdvertisingId();
		if (!_deviceId.IsEmpty())
			return _deviceId;
		LOG("Warning: FPlatformMisc::GetUniqueAdvertisingId() returned an empty string.");

		// Try FPlatformMisc::GetMachineId()
		FGuid MachineId = FPlatformMisc::GetMachineId();
		_deviceId = MachineId.ToString(EGuidFormats::Digits);
		if (!_deviceId.IsEmpty())
			return _deviceId;
		LOG("Warning: FPlatformMisc::GetMachineId() returned an empty string.");

		// Try FPlatformMisc::GetHashedMacAddressString()
		_deviceId = FPlatformMisc::GetHashedMacAddressString();
		if (!_deviceId.IsEmpty())
			return _deviceId;
		LOG("Warning: FPlatformMisc::GetHashedMacAddressString() returned an empty string.");

		// Finally, try FPlatformMisc::GetOperatingSystemId()
		_deviceId = FPlatformMisc::GetOperatingSystemId();
		if (!_deviceId.IsEmpty())
			return _deviceId;
		LOG("Warning: FPlatformMisc::GetOperatingSystemId() returned an empty string.");
		LOG("Error: Faild to generate a unique device ID");

		return _deviceId;
	}


public:
	RegistrationClient(AR51::PlatformType platform, bool isDGS, int port, int discoveryPort) :
		Platform(platform),
		IsDGS(isDGS),
		LocalIp(UNetworkUtils::GetLocalIp()),
		Port(port),
		DiscoveryPort(discoveryPort)
	{

		LOG("Registration: Local Addrses: %s", *LocalIp);
	}

	~RegistrationClient() {
		if (_isRunning) Stop();
		if (_socket != nullptr) {
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(_socket);
		}
	}
};
