#include "AnchorServiceComponent.h"
#include <UnrealThread.h>
#include <Debug.h>
#include <UnityAdapter.h>
#include <BoundaryAdapterFactory.h>
#include "ProceduralMeshComponent.h"
#include <CameraAnchorComponent.h>

using namespace AR51;
using namespace std;
using namespace grpc;

using Boundary = UAnchorServiceComponent::Boundary;

const TArray<AActor*> UAnchorServiceComponent::GetAnchors() const {
	TArray<AActor*> anchors;
	_anchorById.GenerateValueArray(anchors);
	return anchors;
}

AActor* UAnchorServiceComponent::GetAnchorById(const std::string& id) const { return GetAnchorById(ToFString(id)); }

AActor* UAnchorServiceComponent::GetAnchorById(const FString& id) const {
	auto it = _anchorById.Find(id);
	return it ? *it : nullptr;
}

void UAnchorServiceComponent::AnchorToWorldSpace(FVector& position) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	auto anchor = TTransform(AAR51SDK::Instance()).GetLocalToWorldMatrix();
	position = anchor.TransformPosition(position);
}

void UAnchorServiceComponent::AnchorToWorldSpace(TArray<FVector>& positions) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	auto anchor = TTransform(AAR51SDK::Instance()).GetLocalToWorldMatrix();

	for (size_t i = 0; i < positions.Num(); i++)
	{
		positions[i] = anchor.TransformPosition(positions[i]);
	}
}

void UAnchorServiceComponent::AnchorToWorldSpace(FQuat& rotation) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	rotation = TTransform(AAR51SDK::Instance()).GetRotation() * rotation;
}


void UAnchorServiceComponent::WorldToAnchorSpace(FVector& position) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}

	auto anchor = TTransform(AAR51SDK::Instance()).GetWorldToLocalMatrix();
	position = anchor.TransformPosition(position);
}

void UAnchorServiceComponent::WorldToAnchorSpace(TArray<FVector>& positions) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	auto anchor = TTransform(AAR51SDK::Instance()).GetWorldToLocalMatrix();

	for (size_t i = 0; i < positions.Num(); i++)
	{
		positions[i] = anchor.TransformPosition(positions[i]);
	}
}

void UAnchorServiceComponent::WorldToAnchorSpace(FQuat& rotation) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	rotation = TTransform(AAR51SDK::Instance()).GetRotation().Inverse() * rotation;
}

void UAnchorServiceComponent::WorldToAnchorSpace(TArray<FQuat>& rotations) {
	if (!AAR51SDK::Instance()) {
		LOG("Error: SDK is null");
		return;
	}
	auto anchor = TTransform(AAR51SDK::Instance()).GetRotation().Inverse();

	for (size_t i = 0; i < rotations.Num(); i++)
	{
		rotations[i] = anchor * rotations[i];
	}
}

FTransform UAnchorServiceComponent::GetAnchorTransform() {
	if (!Instance())
		return FTransform::Identity;
	auto anchors = Instance()->GetAnchors();
	if (anchors.Num() == 0) return FTransform::Identity;
	return TTransform(anchors[0]).GetLocalToWorldTransform();

}

Boundary UAnchorServiceComponent::CreateBoundary(const FString& id, const TArray<FVector>& points)
{
	auto boundary = BoundaryBP == nullptr ? nullptr : UnityAdapter::Instantiate(GetOwner(), BoundaryBP);
	if (boundary)
		UnityAdapter::SetActive(boundary, true);

	//if (points.Num()) {
	//	auto mesh = UnityAdapter::EnsureComponent<UProceduralMeshComponent>(boundary);
	//	//mesh->SetMaterial(0, BoundaryMaterial.Get().inter);
	//	boundary->Rename(*id);

	//	TArray<FVector> vertices;
	//	TArray<FVector> normals;
	//	TArray<int32> Triangles;
	//	TArray<FVector2D> UV0;
	//	TArray<FProcMeshTangent> tangents;
	//	TArray<FColor> vertexColors;

	//	auto du = 1.f / (points.Num() - 1);

	//	for (auto i = 0; i < points.Num(); i++)
	//	{
	//		auto nextIndex = (i + 1) % points.Num();
	//		auto normal = FVector::CrossProduct((points[nextIndex] - points[i]).GetSafeNormal(), FVector::UpVector).GetSafeNormal();

	//		// bottom vertex
	//		vertices.Add(points[i]);
	//		normals.Add(normal);
	//		tangents.Add({ 0.f, 1.f, 0.f });

	//		// top vertex
	//		vertices.Add(points[i] + BoundaryHeight * FVector::UpVector);
	//		normals.Add(normal);
	//		tangents.Add({ 0.f, 1.f, 0.f });

	//		UV0.Add({ i * du , 0 });
	//		UV0.Add({ i * du , 1 });

	//		vertexColors.Add(FColor::White);
	//		vertexColors.Add(FColor::White);
	//		if (i == points.Num() - 1)
	//			break;

	//		Triangles.Add(i * 2 + 0);
	//		Triangles.Add(i * 2 + 2);
	//		Triangles.Add(i * 2 + 1);

	//		Triangles.Add(i * 2 + 1);
	//		Triangles.Add(i * 2 + 2);
	//		Triangles.Add(i * 2 + 3);
	//	}

	//	mesh->CreateMeshSection(0, vertices, Triangles, normals, UV0, vertexColors, tangents, false);
	//}

	Boundary b(id, boundary, points);
	_boundaryById.Add(id, b);
	return b;
}

void UAnchorServiceComponent::RefreshNativeBoundary()
{
	LOG("RefreshingNativeBoundary");
	auto adapter = BoundaryAdapterFactory::GetAdapter();

	if (adapter) {
		auto boundaryId = adapter->GetBoundaryName();

		auto existingBoundary = _boundaryById.Find(boundaryId);
		if (existingBoundary && (*existingBoundary).Actor) (*existingBoundary).Actor->Destroy();

		auto points = adapter->GetPoints();
		CreateBoundary(boundaryId, points);
		LOG("Refresh native boundary completed: Name/Id=%s, Points=%d", *boundaryId, points.Num());
	}
	else {
		LOG("NativeBoundaryRefresh Completed! Warning: No boundary adapter found");
	}
}

const TArray<Boundary> UAnchorServiceComponent::GetBoundaries() const {
	TArray<Boundary> boundaries;
	_boundaryById.GenerateValueArray(boundaries);
	return boundaries;
}

const TArray<FVector> UAnchorServiceComponent::GetBoundaryPoints(const FString& id) {
	auto adapter = BoundaryAdapterFactory::GetAdapter();
	auto adapterBoundaryName = adapter ? adapter->GetBoundaryName() : "";
	auto boundaryId = id.IsEmpty() ? adapterBoundaryName : id;

	if (boundaryId == adapterBoundaryName) RefreshNativeBoundary();

	auto bounday = FindBoundary(boundaryId);

	return bounday.Points;
}
Boundary UAnchorServiceComponent::FindBoundary(const std::string& id) { return FindBoundary(ToFString(id)); }

Boundary UAnchorServiceComponent::FindBoundary(const FString& id) {

	auto adapter = BoundaryAdapterFactory::GetAdapter();
	auto boundaryId = (id.IsEmpty() && adapter) ? adapter->GetBoundaryName() : id;
	if (adapter && boundaryId == adapter->GetBoundaryName())
		RefreshNativeBoundary();

	auto it = _boundaryById.Find(boundaryId);
	return it ? *it : Boundary::Empty;
}


UAnchorServiceComponent::UAnchorServiceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UAnchorServiceComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAnchorServiceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (IsAutoSyncCameraToAnchor) {

		auto camera = UnityAdapter::GetMainCameraTransform();
		if (camera.IsValid()) {
			auto camParent = camera.GetParent();

			if (camParent.IsValid()) {
				auto cameraToCVS = GetAnchorTransform().Inverse();
				camParent.SetLocalPosition(cameraToCVS.GetLocation());
				camParent.SetLocalRotation(cameraToCVS.GetRotation());
				camParent.SetLocalScale(cameraToCVS.GetScale3D());
			}
			else {
				LOG("Error: Main camera has not parent scene component.");
			}
		}
		else {
			LOG("Error: cannot find main camera.");
		}
	}
}


Status UAnchorServiceComponent::CreateAnchor(ServerContext* context, const CreateAnchorRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return CreateAnchor(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto id = ToFString(request->anchorid());
	auto position = UnityAdapter::ToUnreal(request->position(), UnityAdapter::TO_UNREAL_UNIT_SCALE);
	auto rotation = UnityAdapter::ToUnreal(request->rotation());

	auto anchor = UnityAdapter::Instantiate(GetOwner(), AnchorBP, position, rotation);
	anchor->Rename(*id);
	_anchorById.Add(id, anchor);
	UnityAdapter::SetActive(anchor, request->visible());
	UnityAdapter::SetActive(anchor, true);

	LOG("AnchorService has created anchor: %s in P : %s, R : %s.", *id, *position.ToString(), *rotation.ToString());

	return Status::OK;
}

Status UAnchorServiceComponent::DeleteAnchor(ServerContext* context, const AnchorRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return DeleteAnchor(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto id = ToFString(request->anchorid());
	AActor* anchor = GetAnchorById(id);
	if (anchor) {
		_anchorById.Remove(id);
		anchor->Destroy();
	}
	return Status::OK;
}

Status UAnchorServiceComponent::GetAnchorVisibility(ServerContext* context, const AnchorRequest* request, BoolMessage* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetAnchorVisibility(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto anchor = GetAnchorById(request->anchorid());
	response->set_value(anchor != nullptr && UnityAdapter::GetActive(anchor));
	return Status::OK;
}

Status UAnchorServiceComponent::SetAnchorVisibility(ServerContext* context, const SetAnchorVisiliblityRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetAnchorVisibility(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto anchor = GetAnchorById(request->anchorid());
	//UnityAdapter::SetActive(anchor, request->visible());
	return Status::OK;
}

Status UAnchorServiceComponent::TapToPlaceAnchor(ServerContext* context, const AnchorRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return TapToPlaceAnchor(context, request, response); }, Status::CANCELLED);

	LOG("GRPC"); 
	
	return NotImplimented(__FUNCTION__);
}

Status UAnchorServiceComponent::DownloadAnchor(ServerContext* context, const AnchorRequest* request, ServerWriter<AnchorBytes>* writer)
{
	LOG("GRPC");
	return NotImplimented(__FUNCTION__);
}

Status UAnchorServiceComponent::UploadAnchor(ServerContext* context, ServerReader<AnchorBytes>* reader, Empty* response)
{
	LOG("GRPC");
	return NotImplimented(__FUNCTION__);
}

Status UAnchorServiceComponent::GetAnchorIds(ServerContext* context, const Empty* request, AnchorIdsReply* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetAnchorIds(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto& p : _anchorById) {
		auto id = ToString(p.Key);
		auto visible = UnityAdapter::GetActive(p.Value);
		response->add_anchorids(id);
		if (visible) response->add_visibleanchorids(id);
	}
	return Status::OK;
}

Status UAnchorServiceComponent::SetAllAnchorVisibility(ServerContext* context, const BoolMessage* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return SetAllAnchorVisibility(context, request, response); }, Status::CANCELLED);

	for (auto& p : _anchorById) UnityAdapter::SetActive(p.Value, request->value());

	return Status::OK;
}

Status UAnchorServiceComponent::Clear(ServerContext* context, const Empty* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return Clear(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto& p : _anchorById) p.Value->Destroy();
	_anchorById.Empty();

	return Status::OK;
}

// ===================================================== BOUNDARIES =============================================================


Status UAnchorServiceComponent::GetGuardian(ServerContext* context, const GuardianRequest* request, GetGuardianResponse* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetGuardian(context, request, response); }, Status::CANCELLED);

	RefreshNativeBoundary();
	auto boundary = FindBoundary(request->guardianid());

	response->set_guardianid(request->guardianid());
	auto points = boundary.Points;

	if (points.Num() == 0) {
		points.Add({ 100.f, 100.f, 0 });
		points.Add({ 100.f, -100.f, 0 });
		points.Add({ -100.f, 100.f, 0 });
		points.Add({ -100.f, -100.f, 0 });
		points.Add({ -100.f, -50.f, 0 });
	}
	response->set_points(UnityAdapter::ToUnityBytestring(points, UnityAdapter::TO_UNITY_UNIT_SCALE));


	LOG("GetGuardian: %s with %d points", *ToFString(request->guardianid()), boundary.Points.Num());

	return Status::OK;
}

Status UAnchorServiceComponent::GetGuardianIds(ServerContext* context, const Empty* request, GetGuardianIdsResponse* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return GetGuardianIds(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto& p : _boundaryById)
		response->add_guardianids(ToString(p.Key));
	return Status::OK;
}

Status UAnchorServiceComponent::CreateGuardian(ServerContext* context, const CreateGuardianRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return CreateGuardian(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto id = ToFString(request->guardianid());
	auto points = Converter::ToTArray<FVector>(request->points());
	UnityAdapter::ToUnrealInplace(points, UnityAdapter::TO_UNREAL_UNIT_SCALE);
	CreateBoundary(id, points);
	return Status::OK;
}

Status UAnchorServiceComponent::DestroyGuardian(ServerContext* context, const GuardianRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return DestroyGuardian(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	auto boundary = FindBoundary(request->guardianid());
	if (boundary.Actor) {
		_boundaryById.Remove(boundary.Id);
		boundary.Actor->Destroy();
	}
	return Status::OK;
}

Status UAnchorServiceComponent::ClearGuardians(ServerContext* context, const Empty* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return ClearGuardians(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto p : _boundaryById)
		if (p.Value.Actor) p.Value.Actor->Destroy();
	_boundaryById.Empty();

	RefreshNativeBoundary();
	return Status::OK;
}

Status UAnchorServiceComponent::ShowGuardian(ServerContext* context, const GuardianRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return ShowGuardian(context, request, response); }, Status::CANCELLED);

	LOG("GRPC"); 
	
	auto boundary = FindBoundary(request->guardianid());
	if (boundary.Actor) UnityAdapter::SetActive(boundary.Actor, true);
	return Status::OK;
}

Status UAnchorServiceComponent::HideGuardian(ServerContext* context, const GuardianRequest* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return HideGuardian(context, request, response); }, Status::CANCELLED);

	LOG("GRPC"); 
	
	auto boundary = FindBoundary(request->guardianid());
	if (boundary.Actor) UnityAdapter::SetActive(boundary.Actor, false);
	return Status::OK;
}

Status UAnchorServiceComponent::ShowAllGuardians(ServerContext* context, const Empty* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return ShowAllGuardians(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto p : _boundaryById)
		if (p.Value.Actor) UnityAdapter::SetActive(p.Value.Actor, true);

	return Status::OK;
}

Status UAnchorServiceComponent::HideAllGuardians(ServerContext* context, const Empty* request, Empty* response)
{
	if (!UnrealThread::IsMainThread()) return UnrealThread::Invoke<Status>([&]() { return HideAllGuardians(context, request, response); }, Status::CANCELLED);

	LOG("GRPC");
	for (auto p : _boundaryById)
		if (p.Value.Actor) UnityAdapter::SetActive(p.Value.Actor, false);

	return Status::OK;
}

UAnchorServiceComponent::Boundary::Boundary(const FString& id, AActor* actor, const TArray<FVector>& points)
{
	Id = id;
	Actor = actor;
	Points = points;
}
UAnchorServiceComponent::Boundary UAnchorServiceComponent::Boundary::Empty = { "", nullptr, TArray<FVector>() };