#pragma once

#include "CoreMinimal.h"
#include "ISingleton.h"
#include "Debug.h"
#include <string>
#include "unreal.grpc.h"

template <typename T>
class IServiceComponent : public ISingleton<T>
{
public:
	IServiceComponent() {}

protected:

	grpc::Status NotImplimented(const std::string& method)
	{
		auto msg = method + " not supported on Unreal c++ SDK";
		LOG("%s", msg.c_str());
		return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, msg);
	}
};
