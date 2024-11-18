// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <string>

class AR51SDK_API TTransform
{
private:
	AActor* _owner{ nullptr };
	USceneComponent* _sc{ nullptr };

public:
	TTransform(AActor* owner);
	TTransform(USceneComponent* sc);

private:
	void SetLocalToWorld(const FMatrix& value);
	void SetLocalToWorld(const FVector* p, const FQuat* r, const FVector* s);

	void SetLocalToParent(const FMatrix& value);
	void SetLocalToParent(const FVector* p, const FQuat* r, const FVector* s);

public:
	/// <summary>
	/// Determines if the TTransform instance has a valid underlying scene component.
	/// </summary>
	/// <returns>Returns true if the TTransform is associated with a valid scene component; otherwise, false.</returns>
	bool IsValid() const
	{
		return _owner != nullptr &&
			_sc != nullptr &&
			TSoftObjectPtr<AActor>(_owner).IsValid() &&
			TSoftObjectPtr<USceneComponent>(_sc).IsValid();
	}

	std::string GetName();
	void SetName(const std::string& name);

	FTransform GetLocalToWorldTransform() const;
	void SetLocalToWorldTransform(const FTransform& value);
	
	FTransform GetLocalToParentTransform() const;
	void SetLocalToParentTransform(const FTransform& value);

	FVector GetPosition() const;
	void SetPosition(const FVector& value);

	FQuat GetRotation() const;
	void SetRotation(const FQuat& value);

	FVector GetEulerAngles() const;
	void SetEulerAngles(const FVector& value);

	FVector GetLossyScale() const;
	void SetLossyScale(const FVector& value);


	FVector GetLocalPosition() const;
	void SetLocalPosition(const FVector& value);

	FQuat GetLocalRotation() const;
	void SetLocalRotation(const FQuat& value);

	FVector GetLocalEulerAngles() const;
	void SetLocalEulerAngles(const FVector& value);

	FVector GetLocalScale() const;
	void SetLocalScale(const FVector& value);

	FMatrix GetLocalToWorldMatrix() const;
	void SetLocalToWorldMatrix(const FMatrix& value);

	FMatrix GetWorldToLocalMatrix() const;
	void SetWorldToLocalMatrix(const FMatrix& value);

	FMatrix GetLocalToParentMatrix() const;
	void SetLocalToParentMatrix(const FMatrix& value);

	TTransform GetParent() const { return TTransform(_sc->GetAttachParent()); }
	void SetParent(const TTransform& parent) { _sc->AttachToComponent(parent._sc, FAttachmentTransformRules::KeepWorldTransform); }

	TArray<TTransform> GetChildren() const;

	int GetChildrenCount() const;

	TTransform GetRoot() const;

	TArray<TTransform> DFS();
};
