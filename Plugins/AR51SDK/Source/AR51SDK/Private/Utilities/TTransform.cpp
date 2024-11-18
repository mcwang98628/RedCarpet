#include "TTransform.h"
#include "UnityAdapter.h"

TTransform::TTransform(AActor* owner) {
	if (!owner){
		LOG("Error: owner is null.");
		return;
	}
	_owner = owner;
	_sc = UnityAdapter::GetComponent<USceneComponent>(owner);
}
TTransform::TTransform(USceneComponent* sc) {
	if (!sc) {
		LOG("Error: USceneComponent is null.");
		return;
	}
	_owner = sc->GetOwner();
	_sc = sc;
}

void TTransform::SetLocalToWorld(const FMatrix& value) { SetLocalToWorldTransform(FTransform(value)); }

void TTransform::SetLocalToWorld(const FVector* p, const FQuat* r, const FVector* s) {
	auto t = _sc->GetComponentTransform();
	if (p)t.SetLocation(*p);
	if (r)t.SetRotation(*r);
	if (s)t.SetScale3D(*s);
	SetLocalToWorldTransform(t);
}

void TTransform::SetLocalToParent(const FMatrix& value) { SetLocalToParentTransform(FTransform(value)); }

void TTransform::SetLocalToParent(const FVector* p, const FQuat* r, const FVector* s) {
	auto t = _sc->GetRelativeTransform();
	if (p)t.SetLocation(*p);
	if (r)t.SetRotation(*r);
	if (s)t.SetScale3D(*s);
	SetLocalToParentTransform(t);
}

std::string TTransform::GetName() { return ToString(_owner->GetName()); }
void TTransform::SetName(const std::string& name) { _owner->Rename(*ToFString(name)); }

FTransform TTransform::GetLocalToWorldTransform() const { return _sc->GetComponentTransform(); }
void TTransform::SetLocalToWorldTransform(const FTransform& value) { _sc->SetWorldTransform(value); }

FTransform TTransform::GetLocalToParentTransform() const { return _sc->GetRelativeTransform(); }
void TTransform::SetLocalToParentTransform(const FTransform& value) { _sc->SetRelativeTransform(value); }

FVector TTransform::GetPosition() const { return _sc->GetComponentLocation(); }
void TTransform::SetPosition(const FVector& value) { return _sc->SetWorldLocation(value); }

FQuat TTransform::GetRotation() const { return _sc->GetComponentRotation().Quaternion(); }
void TTransform::SetRotation(const FQuat& value) { return _sc->SetWorldRotation(value); }

FVector TTransform::GetEulerAngles() const { return GetRotation().Euler(); }
void TTransform::SetEulerAngles(const FVector& value) { return SetRotation(FQuat::MakeFromEuler(value)); }

FVector TTransform::GetLossyScale() const { return _sc->GetComponentScale(); }
void TTransform::SetLossyScale(const FVector& value) { return _sc->SetWorldScale3D(value);; }

FVector TTransform::GetLocalPosition() const { return _sc->GetRelativeLocation(); }
void TTransform::SetLocalPosition(const FVector& value) { return _sc->SetRelativeLocation(value); }

FQuat TTransform::GetLocalRotation() const { return _sc->GetRelativeRotation().Quaternion(); }
void TTransform::SetLocalRotation(const FQuat& value) { return _sc->SetRelativeRotation(value); }

FVector TTransform::GetLocalEulerAngles() const { return GetLocalRotation().Euler(); }
void TTransform::SetLocalEulerAngles(const FVector& value) { return SetLocalRotation(FQuat::MakeFromEuler(value)); }

FVector TTransform::GetLocalScale() const { return _sc->GetRelativeScale3D(); }
void TTransform::SetLocalScale(const FVector& value) { return _sc->SetRelativeScale3D(value); }

FMatrix TTransform::GetLocalToWorldMatrix() const { return GetLocalToWorldTransform().ToMatrixWithScale(); }
void TTransform::SetLocalToWorldMatrix(const FMatrix& value) { SetLocalToWorld(value); }

FMatrix TTransform::GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().Inverse(); }
void TTransform::SetWorldToLocalMatrix(const FMatrix& value) { SetLocalToWorldMatrix(value.Inverse()); }

FMatrix TTransform::GetLocalToParentMatrix() const { return GetLocalToParentTransform().ToMatrixWithScale(); }
void TTransform::SetLocalToParentMatrix(const FMatrix& value) { SetLocalToParent(value); }

TArray<TTransform> TTransform::GetChildren() const {
	TArray<TTransform> children;
	for (auto child : _sc->GetAttachChildren())
		children.Add(TTransform(child));
	return children;
}

int TTransform::GetChildrenCount() const { return _sc->GetAttachChildren().Num(); }

TTransform TTransform::GetRoot() const { return TTransform(_sc->GetAttachmentRoot()); }

TArray<TTransform> TTransform::DFS() {
	TArray<TTransform> dfs;
	dfs.Add(*this);
	for (auto& child : GetChildren())
		for (auto& t : child.DFS())
			dfs.Add(t);

	return dfs;
}
