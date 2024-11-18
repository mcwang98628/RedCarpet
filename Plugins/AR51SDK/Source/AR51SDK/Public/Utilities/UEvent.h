// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <functional>
#include <mutex>
#include "Debug.h"
#include "Converter.h"


template <typename TSender, typename TArgs>
class UEvent
{
	using Handler = const std::function<void(TSender, TArgs)>&;
private:
	std::vector<std::function<void(TSender, TArgs)>> _invocationList;
	mutable std::mutex _mutex;
public:
	UEvent() : _invocationList(), _mutex() {}
	UEvent(UEvent&) = delete;
	UEvent& operator=(UEvent const&) = delete;
	~UEvent() = default;

	void operator+=(Handler handler) {
		std::lock_guard<std::mutex> lock(_mutex);
		_invocationList.push_back(handler);
	}

	void operator-=(Handler handler) {
		// todo fix this
		//std::lock_guard<std::mutex> lock(_mutex);
		//auto it = std::find_if(_invocationList.begin(), _invocationList.end(), [&handler](Handler h) {
		//	return h.target<void(TSender, TArgs)>() == handler.target<void(TSender, TArgs)>();
		//});
		//if (it != _invocationList.end()) _invocationList.erase(it);
	}

	void Invoke(TSender sender, TArgs args) const {
		std::lock_guard<std::mutex> lock(_mutex);
		for (auto& handler : _invocationList) {

			handler(sender, args);
			//try
			//{
			//	handler(sender, args);
			//}
			//catch (const std::exception& ex)
			//{
			//	LOG("Failed in Invoke Event! Error: %s", *ToFString(ex.what()));
			//}
		}
	}

	void operator()(TSender sender, TArgs args) const { Invoke(sender, args); }
};