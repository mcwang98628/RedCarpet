#include "EngineServiceComponent.h"
#include "Converter.h"
#include "UnityAdapter.h"
#include "UnrealThread.h"
#include "Debug.h"
#include <chrono>
#include "AnchorServiceComponent.h"

using namespace AR51;
using namespace grpc;

UEngineServiceComponent::UEngineServiceComponent() { PrimaryComponentTick.bCanEverTick = true; }


Status UEngineServiceComponent::GetPyTime(ServerContext* context, const Empty* request, Float64Message* response) {
	auto pyTime = std::chrono::duration_cast<std::chrono::duration<double>>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	response->set_value(pyTime + _pyTimeOffset);
	return Status::OK;
}

Status UEngineServiceComponent::GetPyTimeOffset(ServerContext* context, const Empty* request, Float64Message* response) {
	response->set_value(_pyTimeOffset);
	return Status::OK;
}

Status UEngineServiceComponent::SetPyTimeOffset(ServerContext* context, const Float64Message* request, Empty* response) {
	_pyTimeOffset = request->value();
	return Status::OK; 
}

Status UEngineServiceComponent::Instantiate(ServerContext* context, const InstantiateRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return Instantiate(context, request, response); }, Status::CANCELLED);

	auto parent = UnityAdapter::GetActorById(request->parentid());
	auto name = ToFString(request->name());
	for (auto& bp : InstanceBlueprints) {
		if (bp->GetName() == name) {
			auto instance = UnityAdapter::Instantiate(GetOwner(), bp);
			TTransform t(instance);

			if (parent) t.SetParent(TTransform(parent));

			t.SetPosition(UnityAdapter::ToUnreal(request->position()));
			t.SetRotation(UnityAdapter::ToUnreal(request->rotation()));
			t.SetLocalScale(UnityAdapter::ToUnreal(request->scale()));

			UnityAdapter::SetMaterialColor(instance, "_Color", request->color());

			_instances.Add(instance);
			response->set_id((int64_t)instance);

			// break cause we found the blueprint to instantiate
			break;
		}
	}
 
	return Status::OK; 
}

Status UEngineServiceComponent::FindGameObject(ServerContext* context, const NameRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return FindGameObject(context, request, response); }, Status::CANCELLED);

	auto actor = UnityAdapter::FindActor(this, request->name());
	response->set_id(actor ? (int64_t)actor : -1);
 
	return Status::OK;
}

Status UEngineServiceComponent::FindAnchorGameObject(ServerContext* context, const AnchorRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return FindAnchorGameObject(context, request, response); }, Status::CANCELLED);
 
	auto anchorId = ToFString(request->anchorid());
	auto anchor = UAnchorServiceComponent::Instance()->GetAnchorById(anchorId);

	auto goId = anchor ? (int64_t)anchor : -1;
	response->set_id(goId);

	LOG("FindAnchorGameObject(anchorId=%s) found anchor with entity id: %d", *anchorId, goId);

	return Status::OK;
}

Status UEngineServiceComponent::GetMainCamera(ServerContext* context, const Empty* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetMainCamera(context, request, response); }, Status::CANCELLED);
 

	auto camera = UnityAdapter::GetMainCamera();
	
	if (!camera) return ActorMissing(__FUNCTION__, -1);

	response->set_id((int64_t)camera);
	return Status::OK; 
}

Status UEngineServiceComponent::Destroy(ServerContext* context, const ObjectIdMessage* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return Destroy(context, request, response); }, Status::CANCELLED);
 

	auto id = request->id();
	auto actor = UnityAdapter::GetActorById(id);
	if (!actor) return ActorMissing(__FUNCTION__, id);
	actor->Destroy();
	return Status::OK; 
}

Status UEngineServiceComponent::CreatePrimitive(ServerContext* context, const CreatePrimitiveRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return CreatePrimitive(context, request, response); }, Status::CANCELLED);

	auto actor = UnityAdapter::NewGameObject(this, "primitive" + std::to_string(_instances.Num()));
	response->set_id((int64_t)actor);
	_instances.Add(actor);

	return Status::OK;
}

Status UEngineServiceComponent::NewGameObject(ServerContext* context, const NameRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return NewGameObject(context, request, response); }, Status::CANCELLED);

	auto actor = UnityAdapter::NewGameObject(this, request->name());
	response->set_id((int64_t)actor);
	_instances.Add(actor);
 
	return Status::OK; 
}

Status UEngineServiceComponent::ClearInstances(ServerContext* context, const Empty* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return ClearInstances(context, request, response); }, Status::CANCELLED);

	for (auto instance : _instances) {
		TSoftObjectPtr<AActor> p(instance);
		if (p.IsValid()) instance->Destroy();
	}
	_instances.Empty();

	return Status::OK; 
}

Status UEngineServiceComponent::SetParent(ServerContext* context, const SetParentRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetParent(context, request, response); }, Status::CANCELLED);
 
	auto childId = request->id();
	auto parentId = request->parentid();

	auto actor = UnityAdapter::GetActorById(childId);
	auto parent = UnityAdapter::GetActorById(parentId);

	if (!actor) return ActorMissing(__FUNCTION__, childId);
	if (!parent) return ActorMissing(__FUNCTION__, parentId);

	TTransform(actor).SetParent(TTransform(parent));

	return Status::OK; 
}


Status UEngineServiceComponent::ActorMissing(const std::string& method, int64_t id) {
	auto errorMsg = method + "() Error: Failed accessing object that was released with id: " + std::to_string(id);
	LOG("%s", *ToFString(errorMsg));
	return Status(StatusCode::UNAVAILABLE, errorMsg);
}

Status UEngineServiceComponent::GetName(ServerContext* context, const ObjectIdMessage* request, NameRequest* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetName(context, request, response); }, Status::CANCELLED);

	auto id = request->id();
	auto actor = UnityAdapter::GetActorById(id);

	if (!actor) return ActorMissing(__FUNCTION__, id);

	response->set_name(ToString(actor->GetName()));
	return Status::OK; 
}

Status UEngineServiceComponent::SetName(ServerContext* context, const SetNameRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetName(context, request, response); }, Status::CANCELLED);

	auto id = request->id();
	auto actor = UnityAdapter::GetActorById(id);

	if (!actor) return ActorMissing(__FUNCTION__, id);

	TTransform(actor).SetName(request->name());
	
	return Status::OK; 
}

Status UEngineServiceComponent::GetActive(ServerContext* context, const ObjectIdMessage* request, ActiveMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetActive(context, request, response); }, Status::CANCELLED);

	auto id = request->id();
	auto actor = UnityAdapter::GetActorById(id);

	if (!actor) return ActorMissing(__FUNCTION__, id);

	auto isActive = UnityAdapter::GetActive(actor);
	response->set_isactive(isActive);
	return Status::OK; 
}

Status UEngineServiceComponent::SetActive(ServerContext* context, const ActiveMessage* request, ActiveMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetActive(context, request, response); }, Status::CANCELLED);
 
	auto id = request->id();
	auto actor = UnityAdapter::GetActorById(id);
	
	if (!actor) return ActorMissing(__FUNCTION__, id);

	UnityAdapter::SetActive(actor, request->isactive());

	return Status::OK; 
}

bool UEngineServiceComponent::TryGetTransform(int64_t id, AR51::TransformTypes type, std::string& outBytestring) const {
	auto actor = UnityAdapter::GetActorById(id);
	if (!actor) return false;

	auto transform = UnityAdapter::GetTransform(actor);

	switch (type) {
	case TransformTypes::position:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetPosition(), UnityAdapter::TO_UNITY_UNIT_SCALE);
		break;
	case TransformTypes::localPosition:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLocalPosition(), UnityAdapter::TO_UNITY_UNIT_SCALE);
		break;
	case TransformTypes::rotation:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetRotation());
		break;
	case TransformTypes::localRotation:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLocalRotation());
		break;
	case TransformTypes::localScale:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLocalScale());
		break;
	case TransformTypes::lossyScale:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLossyScale());
		break;
	case TransformTypes::eulerAngles:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetEulerAngles());
		break;
	case TransformTypes::localEulerAngles:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLocalEulerAngles());
		break;
	case TransformTypes::worldToLocalMatrix:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetWorldToLocalMatrix(), UnityAdapter::TO_UNITY_UNIT_SCALE);
		break;
	case TransformTypes::localToWorldMatrix:
		outBytestring = UnityAdapter::ToUnityBytestring(transform.GetLocalToWorldMatrix(), UnityAdapter::TO_UNITY_UNIT_SCALE);
		break;
	} // end switch;

	return true;
}

Status UEngineServiceComponent::GetTransform(ServerContext* context, const GetTransformRequest* request, TransformMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetTransform(context, request, response); }, Status::CANCELLED);
 

	std::string bytestring;
	auto id = request->gameobjectid();
	auto actor = UnityAdapter::GetActorById(id);

	if (!TryGetTransform(id, request->type(), bytestring)) {
		auto errorMsg = "EngineSerivce::GetTransform() Error: Failed accesing object that was released with id: " + std::to_string(id);
		LOG("%s", *FString(errorMsg.c_str()));
		return Status(StatusCode::UNAVAILABLE, errorMsg);
	}

	response->set_value(bytestring);
	return Status::OK;
}

Status UEngineServiceComponent::SetTransform(ServerContext* context, const SetTransformRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetTransform(context, request, response); }, Status::CANCELLED);
 
	std::string errorMsg = "";
	auto id = request->gameobjectid();
	auto bytestring = request->value();
	auto actor = UnityAdapter::GetActorById(id);
	
	if (!actor) return ActorMissing(__FUNCTION__, id);

	auto transform = UnityAdapter::GetTransform(actor);
	switch (request->type()) {
	case TransformTypes::position:
		transform.SetPosition(UnityAdapter::ToUnreal<FVector>(bytestring, UnityAdapter::TO_UNREAL_UNIT_SCALE));
		break;
	case TransformTypes::localPosition:
		transform.SetLocalPosition(UnityAdapter::ToUnreal<FVector>(bytestring, UnityAdapter::TO_UNREAL_UNIT_SCALE));
		break;
	case TransformTypes::rotation:
		transform.SetRotation(UnityAdapter::ToUnreal<FQuat>(bytestring));
		break;
	case TransformTypes::localRotation:
		transform.SetLocalRotation(UnityAdapter::ToUnreal<FQuat>(bytestring));
		break;
	case TransformTypes::localScale:
		transform.SetLocalScale(UnityAdapter::ToUnreal<FVector>(bytestring));
		break;
	case TransformTypes::lossyScale:
		transform.SetLossyScale(UnityAdapter::ToUnreal<FVector>(bytestring));
		break;
	case TransformTypes::eulerAngles:
		transform.SetEulerAngles(UnityAdapter::ToUnreal<FVector>(bytestring));
		break;
	case TransformTypes::localEulerAngles:
		transform.SetLocalEulerAngles(UnityAdapter::ToUnreal<FVector>(bytestring));
		break;
	case TransformTypes::worldToLocalMatrix:
		transform.SetWorldToLocalMatrix(UnityAdapter::ToUnreal<FMatrix>(bytestring, UnityAdapter::TO_UNREAL_UNIT_SCALE));
		break;
	case TransformTypes::localToWorldMatrix:
		auto unityM = Converter::ToStruct<FMatrix>(bytestring);
		auto unrealM = UnityAdapter::ToUnreal<FMatrix>(bytestring, UnityAdapter::TO_UNREAL_UNIT_SCALE);
		transform.SetLocalToWorldMatrix(unrealM);
		break;
	} // end switch
	return Status::OK; 
}

Status UEngineServiceComponent::StreamTransform(ServerContext* context, const GetTransformRequest* request, ServerWriter< TransformMessage>* writer) {
	TransformMessage response;
	std::string bytestring;
	auto id = request->gameobjectid();
	auto type = request->type();
	auto task = Task<bool>::CompletedTask();
	do {

		task = UnrealThread::BeginInvoke<bool>([&]() { return TryGetTransform(id, type, bytestring); });
		task->Wait();
		response.set_value(bytestring);
	} while (task->IsCompleted() && task->GetResult() && writer->Write(response));

	auto status = Status::OK;

	if (!task->IsCompleted()) {
		status = Status(StatusCode::CANCELLED, "EngineService::StreamTransform() canceled!");
	}
	else if (!task->GetResult()) {
		auto errorMsg = "EngineService::StreamTransform() Error: Failed accessing object that was released with id: " + std::to_string(id);
		LOG("%s", *FString(errorMsg.c_str()));
		status = Status(StatusCode::UNAVAILABLE, errorMsg);
	}
 
	return status;
}

Status UEngineServiceComponent::GetComponent(ServerContext* context, const ComponentRequest* request, ObjectIdMessage* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetComponent(context, request, response); }, Status::CANCELLED);
 

	auto id = request->gameobjectid();
	auto componentType = request->componenttype();
	auto component = UnityAdapter::GetComponent(id, componentType);

	auto status = Status::OK;

	if (component) {
		response->set_id((int64_t)component);
	}
	else {
		response->set_id(-1);
		auto errorMsg = FString::Printf(TEXT("EngineService::GetComponent(%d, %s) Error: Failed accessing object that was released!"),
			id, *ToFString(componentType));

		LOG("%s", *errorMsg);
		status = Status(StatusCode::UNAVAILABLE, ToString(errorMsg));
	}

	return status;
}

Status UEngineServiceComponent::GetComponentField(ServerContext* context, const ComponentFieldMessage* request, ComponentFieldMessage* response) {
	return NotImplimented(__FUNCTION__);
}

Status UEngineServiceComponent::SetComponentField(ServerContext* context, const ComponentFieldMessage* request, Empty* response) {
	return NotImplimented(__FUNCTION__);
}

Status UEngineServiceComponent::SetCameraProject(ServerContext* context, const TransformMessage* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetCameraProject(context, request, response); }, Status::CANCELLED);
 

	//auto cam = UnityAdapter::GetMainCamera();
	//auto cameraProjection = UnityAdapter::ToUnreal<FMatrix>(request->value());

	//// todo implement
	return NotImplimented(__FUNCTION__);
	//return Status::OK;
}

Status UEngineServiceComponent::SetRendererEnabled(ServerContext* context, const SetRendererEnabledRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetRendererEnabled(context, request, response); }, Status::CANCELLED);
 

	//auto id = request->gameobjectid();
	//auto actor = UnityAdapter::GetActorById(id);
	//auto enabled = request->enabled();
	//
	//if (!actor) return ActorMissing(__FUNCTION__, id);

	//UnityAdapter::SetVisiblity(actor, enabled);

	return Status::OK; 
}

Status UEngineServiceComponent::SetMaterialTexture(ServerContext* context, const SetMaterialTextureRequest* request, Empty* response) {
	return Status::OK;
}

Status UEngineServiceComponent::SetMaterialInt32(ServerContext* context, const SetMaterialInt32Request* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetMaterialInt32(context, request, response); }, Status::CANCELLED);

	//auto id = request->gameobjectid();
	//auto actor = UnityAdapter::GetActorById(id);

	//if (!actor) return ActorMissing(__FUNCTION__, id);

	//UnityAdapter::SetMaterialField(actor, request->name(), (float)request->value());
	return Status::OK;
}

Status UEngineServiceComponent::SetMaterialFloat(ServerContext* context, const SetMaterialFloatRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetMaterialFloat(context, request, response); }, Status::CANCELLED);
 
	//auto id = request->gameobjectid();
	//auto actor = UnityAdapter::GetActorById(id);

	//if (!actor) return ActorMissing(__FUNCTION__, id);

	//UnityAdapter::SetMaterialField(actor, request->name(), request->value());
	return Status::OK;
}

Status UEngineServiceComponent::SetMaterialColor(ServerContext* context, const SetMaterialColorRequest* request, Empty* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetMaterialColor(context, request, response); }, Status::CANCELLED);
 
	//auto id = request->gameobjectid();
	//auto actor = UnityAdapter::GetActorById(id);
	//
	//if (!actor) return ActorMissing(__FUNCTION__, id);

	//UnityAdapter::SetMaterialColor(actor, request->name(), request->color());
	return Status::OK; 
}

Status UEngineServiceComponent::GetMaterialColor(ServerContext* context, const GetMaterialColorRequest* request, Color* response) {
	// sync to main thread
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetMaterialColor(context, request, response); }, Status::CANCELLED);
 //
	//auto id = request->gameobjectid();
	//auto actor = UnityAdapter::GetActorById(id);
	//
	//if (!actor) return ActorMissing(__FUNCTION__, id);

	//TArray<UPrimitiveComponent*> primitives;
	//actor->GetComponents(primitives);

	//auto color = UnityAdapter::GetMaterialColor(actor, request->name());
	//response->set_r(color.R);
	//response->set_g(color.G);
	//response->set_b(color.B);
	//response->set_a(color.A);

	return Status::OK;
}


void UEngineServiceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEngineServiceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = _instances.Num() - 1; i >= 0; i--)
	{
		auto instance = UnityAdapter::GetActorById((int64_t)_instances[i]);
		if (!instance) _instances.RemoveAt(i);
	}
}
