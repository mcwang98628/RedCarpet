#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include <string>
#include <vector>
#include <SocketSubsystem.h>
#include "TTransform.h"
#include "Converter.h"
#include "unreal.grpc.h"
#include <Kismet\GameplayStatics.h>
#include <Camera\CameraComponent.h>
#include "Materials/MaterialInstanceDynamic.h"
#include "Debug.h"

class UnityAdapter {
public:
	/// <summary>
	/// Unreal units are cm, Unity units are meters, conversion is 100.f
	/// </summary>
	static const float TO_UNREAL_UNIT_SCALE;

	/// <summary>
	/// Unreal units are cm, Unity units are meters, conversion is 0.01f
	/// </summary>
	static const float TO_UNITY_UNIT_SCALE;

	static FLinearColor ParseColor(const FString& colorName);

	static AActor* NewGameObject(UWorld* world, const std::string& name) {
		if (!world) {
			LOG("Error: Cannot spawn new actor, the argument world is null");
			return nullptr;
		}

		auto actor = world->SpawnActor<APawn>(FVector::ZeroVector, FRotator());
		auto sName = ToFString(name);

		if (actor) {
			actor->Rename(*sName);
			EnsureComponent<USceneComponent>(actor);
		}
		else {
			LOG("Error: Failed spawning a new actor with the name: %s", *sName);
		}

		return actor;
	}
	static AActor* NewGameObject(AActor* actor, const std::string& name) { return NewGameObject(actor->GetWorld(), name); }
	static AActor* NewGameObject(UActorComponent* c, const std::string& name) { return NewGameObject(c->GetWorld(), name); }

	static AActor* GetActorById(int64_t id) {
		if (id < 0) return nullptr;
		TSoftObjectPtr<AActor> actor((AActor*)id);
		return actor.IsValid() ? actor.Get() : nullptr;
	}


	/// <summary>
	/// Gets the main camera from the world.
	/// </summary>
	static AActor* GetMainCamera();

	static TTransform GetMainCameraTransform();


	static TTransform GetTransform(UActorComponent* component) { return TTransform(component->GetOwner()); }
	static TTransform GetTransform(AActor* owner, bool overrideMainCamera = true) {
		if (overrideMainCamera && owner == UnityAdapter::GetMainCamera())
			return UnityAdapter::GetMainCameraTransform();

		return TTransform(owner);
	}
	static TTransform GetTransform(USceneComponent* sceneComponent) { return TTransform(sceneComponent); }

	template <typename TComponent>
	static TComponent* GetComponent(int64_t id) { return GetComponent<TComponent>(GetActorById(id)); }

	template <typename TComponent>
	static TComponent* AddComponent(AActor* actor) {
		if (!actor) return nullptr;

		TComponent* component = NewObject<TComponent>(actor);

		if (component)
		{
			component->RegisterComponent();
			component->AttachToComponent(actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			actor->AddOwnedComponent(component);
		}
		return component;
	}

	template <typename TComponent>
	static TComponent* EnsureComponent(AActor* actor) {
		auto component = GetComponent<TComponent>(actor);
		if (!component) {

			component = NewObject<TComponent>(actor);
			component->RegisterComponent();
			actor->AddInstanceComponent(component);
		}
		return component;
	}


	template <typename TComponent>
	static TComponent* AddComponent(UActorComponent* component) { return AddComponent<TComponent>(component->GetOwner()); }

	template <typename TComponent>
	static TComponent* EnsureComponent(UActorComponent* component) { return EnsureComponent<TComponent>(component->GetOwner()); }

	static void SetActive(AActor* actor, bool active) {
		if (!actor) return;
		actor->SetActorHiddenInGame(!active);
		actor->SetActorEnableCollision(active);
		actor->SetActorTickEnabled(active);
	}

	static bool GetActive(AActor* actor) { return actor->IsHidden(); }


	template <typename TComponent>
	static TArray<TComponent*> GetComponents(AActor* actor) {
		TArray<TComponent*> components;
		if (!actor) return components;
		actor->GetComponents(components, false);
		return components;
	}

	template <typename TComponent>
	static TArray<TComponent*> GetComponentsInChildren(AActor* actor) {
		TArray<TComponent*> components;
		if (!actor) return components;
		actor->GetComponents(components, true);
		return components;
	}

	template <typename TComponent>
	static TComponent* GetComponentInChildren(AActor* actor) {
		auto c = GetComponentsInChildren<TComponent>(actor);
		return c.Num() ? c[0] : nullptr;
	}

	template <typename TComponent>
	static TComponent* GetComponent(AActor* actor) {
		if (!actor) return nullptr;
		auto component = actor->GetComponentByClass(TComponent::StaticClass());
		return Cast<TComponent>(component);
	}

	static UActorComponent* GetComponent(int64_t id, const std::string& componentTypeName) {
		auto actor = GetActorById(id);
		if (!actor) return nullptr;

		FName name(componentTypeName.c_str());
		auto components = actor->GetComponents();

		for (auto c : components) {
			if (c->GetFName() == name) return c;
		}
		return nullptr;
	}

	static AActor* Instantiate(AActor* actor, TSubclassOf<AActor> type, FVector position = FVector::ZeroVector, FQuat rotation = FQuat::Identity)
	{
		FActorSpawnParameters SpawnParams;
		auto instance = actor->GetWorld()->SpawnActor<AActor>(type, position, rotation.Rotator(), SpawnParams);
		return instance;
	}

	/// <summary>
	/// Change the coordinates system from Unity XYZ to Unreal ZXY
	/// Unity (X = Right, Y = Up, Z = Forward) -->> Unreal (X = Forward (Z), Y = Right (X), Z = Up (Y)).
	/// </summary>
	static FVector ToUnreal(const FVector& v, float scale = 1.f) { return { v.Z * scale, v.X * scale, v.Y * scale }; }

	/// <summary>
	/// todo Not Implemented
	/// </summary>
	static FQuat ToUnreal(const FQuat& v) { return { v.Z, v.X, v.Y, v.W }; }

	/// <summary>
	/// todo Not Implemented
	/// </summary>
	static FMatrix ToUnreal(const FMatrix& v, float scale = 1.f) {
		FMatrix	Result;

		// x -> y
		Result.M[1][0] = v.M[2][0];
		Result.M[1][1] = v.M[0][0];
		Result.M[1][2] = v.M[1][0];
		Result.M[1][3] = v.M[3][0];

		// y -> z
		Result.M[2][0] = v.M[2][1];
		Result.M[2][1] = v.M[0][1];
		Result.M[2][2] = v.M[1][1];
		Result.M[2][3] = v.M[3][1];

		// z -> x
		Result.M[0][0] = v.M[2][2];
		Result.M[0][1] = v.M[0][2];
		Result.M[0][2] = v.M[1][2];
		Result.M[0][3] = v.M[3][2];

		Result.M[3][0] = v.M[2][3] * scale;
		Result.M[3][1] = v.M[0][3] * scale;
		Result.M[3][2] = v.M[1][3] * scale;
		Result.M[3][3] = v.M[3][3];
		return Result;
	}

	/// <summary>
	/// Change the coordinates system from Unity XYZ to Unreal ZXY
	/// Unity (X = Right, Y = Up, Z = Forward) -->> Unreal (X = Forward (Z), Y = Right (X), Z = Up (Y)).
	/// </summary>
	static void ToUnrealInplace(TArray<FVector>& items, float scale = 1.f) {
		if (scale != 1.f)
			for (size_t i = 0; i < items.Num(); i++) items[i] = scale * ToUnreal(items[i]);
		else
			for (size_t i = 0; i < items.Num(); i++) items[i] = ToUnreal(items[i]);
	}

	static TArray<FVector> ToUnreal(const TArray<FVector>& items, float scale = 1.f) {
		auto clone = items;
		ToUnrealInplace(clone, scale);
		return clone;
	}


	static FVector ToUnity(const FVector& v) { return { v.Y, v.Z, v.X }; }

	static FVector ToUnity(const FVector& v, float scale) { return { v.Y * scale, v.Z * scale, v.X * scale }; }

	static FQuat ToUnity(const FQuat& v) { return { v.Y, v.Z, v.X, v.W }; }

	/// <summary>
	/// todo Not Implemented
	/// </summary>
	static FMatrix ToUnity(const FMatrix& v, float scale = 1.f) {
		FMatrix	Result;

		// row x -> col z
		Result.M[0][2] = v.M[0][0];
		Result.M[1][2] = v.M[0][1];
		Result.M[2][2] = v.M[0][2];
		Result.M[3][2] = v.M[0][3];

		// row y -> col x
		Result.M[0][0] = v.M[1][0];
		Result.M[1][0] = v.M[1][1];
		Result.M[2][0] = v.M[1][2];
		Result.M[3][0] = v.M[1][3];

		// row z -> col y
		Result.M[0][1] = v.M[2][0];
		Result.M[1][1] = v.M[2][1];
		Result.M[2][1] = v.M[2][2];
		Result.M[3][1] = v.M[2][3];

		Result.M[0][3] = v.M[3][0] * scale;
		Result.M[1][3] = v.M[3][1] * scale;
		Result.M[2][3] = v.M[3][2] * scale;
		Result.M[3][3] = v.M[3][3];

		return Result;
	}

	template <typename T>
	static std::string ToUnityBytestring(const T& value) { return Converter::Serialize<T>(ToUnity(value)); }

	template <typename T>
	static std::string ToUnityBytestring(const T& value, float scale) { return Converter::Serialize<T>(ToUnity(value, scale)); }

	template <typename T>
	static std::string ToUnityBytestring(const TArray<T>& unrealArray, float scale = 1.f) {
		TArray<T> unityArray = unrealArray;
		for (size_t i = 0; i < unrealArray.Num(); i++)
		{
			unityArray[i] = ToUnity(unrealArray[i]) * scale;
		}
		return Converter::Serialize(unityArray);
	}

	template <typename T>
	static T ToUnreal(const std::string& bytestring) { return ToUnreal(Converter::ToStruct<T>(bytestring)); }

	template <typename T>
	static T ToUnreal(const std::string& bytestring, float scale) { return ToUnreal(Converter::ToStruct<T>(bytestring), scale); }

	static FLinearColor ToUnreal(const AR51::Color& c) { return FLinearColor(c.r(), c.g(), c.b(), c.a()); }

	static FVector ToUnreal(const AR51::Vector3& v, float scale = 1.f) { return ToUnreal(FVector(v.x(), v.y(), v.z()), scale); }
	static FQuat ToUnreal(const AR51::Quaternion& v) { return ToUnreal(FQuat(v.x(), v.y(), v.z(), v.w())); }

	static bool GetVisiblity(AActor* actor) {
		if (!actor) return false;
		TArray<UPrimitiveComponent*> primitives = GetComponentsInChildren<UPrimitiveComponent>(actor);

		for (auto p : primitives)
			return p->IsVisible();
		return false;
	}
	static void SetVisiblity(AActor* actor, bool visible, bool recursive = false) {
		if (!actor) return;
		TArray<UPrimitiveComponent*> primitives = recursive ?
			GetComponentsInChildren<UPrimitiveComponent>(actor) : GetComponents<UPrimitiveComponent>(actor);

		for (auto p : primitives)
			p->SetVisibility(visible);
	}

	static FName ToUnrealColorFieldName(const std::string& colorFieldName) {
		auto name = colorFieldName == "_Color" ? "BaseColor" : colorFieldName;
		return FName(name.c_str());
	}

	static void SetMaterialColor(AActor* actor, const std::string& colorFieldName, const AR51::Color& color) {
		auto name = ToUnrealColorFieldName(colorFieldName);
		auto c = ToUnreal(color);

		auto primitives = GetComponents<UPrimitiveComponent>(actor);
		for (auto p : primitives) {
			TArray<UMaterialInterface*> materials;
			if (p) p->GetUsedMaterials(materials);
			for (auto material : materials) {
				auto m = (UMaterialInstanceDynamic*)material;
				m->SetVectorParameterValue(name, c);
			}
		}
	}

	static FLinearColor GetMaterialColor(AActor* actor, const std::string& colorFieldName) {
		auto name = ToUnrealColorFieldName(colorFieldName);

		FLinearColor color;
		auto primitives = GetComponents<UPrimitiveComponent>(actor);
		for (auto p : primitives) {
			TArray<UMaterialInterface*> materials;
			if (p) p->GetUsedMaterials(materials);
			for (auto material : materials) {
				auto m = (UMaterialInstanceDynamic*)material;
				m->GetVectorParameterValue(name, color);
				return color;
			}
		}
		return color;
	}

	static void SetMaterialField(AActor* actor, const std::string& name, float value) {

		auto primitives = UnityAdapter::GetComponents<UPrimitiveComponent>(actor);

		auto fname = FName(name.c_str());

		for (auto p : primitives) {
			TArray<UMaterialInterface*> materials;
			if (p) p->GetUsedMaterials(materials);
			for (auto material : materials) {
				auto m = (UMaterialInstanceDynamic*)material;
				m->SetScalarParameterValue(fname, value);
			}
		}
	}

	static std::string GetFullPath(AActor* actor) {
		if (!actor) return "";
		if (!actor->GetParentActor()) return ToString(actor->GetName());
		return GetFullPath(actor->GetParentActor()) + "/" + ToString(actor->GetName());
	}

	static TArray<AActor*> GetAllActors(UActorComponent* component, const std::string& path) { return GetAllActors(component->GetWorld()); }
	static TArray<AActor*> GetAllActors(AActor* actor, const std::string& path) { return GetAllActors(actor->GetWorld()); }
	static TArray<AActor*> GetAllActors(UWorld* world) {
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(world, AActor::StaticClass(), actors);
		return actors;
	}

	static AActor* FindActor(UActorComponent* component, const std::string& path) { return FindActor(component->GetWorld(), path); }
	static AActor* FindActor(AActor* actor, const std::string& path) { return FindActor(actor->GetWorld(), path); }
	static AActor* FindActor(UWorld* world, const std::string& path) {
		AActor* result = nullptr;
		for (auto a : GetAllActors(world)) {
			auto fullPath = GetFullPath(a);
			if (fullPath != path) continue;

			result = a;
			break;
		}
		return result;
	}

	/// <summary>
	/// Returns the first instance of the specified type component in the world level
	/// </summary>
	/// <typeparam name="TComponent">The component's type to search for</typeparam>
	/// <param name="world">The world level to search in</param>
	/// <returns>The first instance of a component of the specified type</returns>
	template <typename TComponent>
	static TComponent* FindComponentByType(UWorld* world) {
		for (auto a : GetAllActors(world)) {
			auto c = GetComponentInChildren<TComponent>(a);
			if (c) return c;
		}
		return nullptr;
	}


	/// <summary>
	/// Returns the all instances of the specified type in the world level
	/// </summary>
	/// <typeparam name="TComponent">The components' type to search for</typeparam>
	/// <param name="world">The world level to search in</param>
	/// <returns>An array of component instances of the specified type</returns>
	template <typename TComponent>
	static TArray<TComponent*> FindComponentsByType(UWorld* world) {
		TArray<TComponent*> components;
		for (auto a : GetAllActors(world)) {
			auto c = GetComponentsInChildren<TComponent>(a);
			components.Append(c);
		}
		return components;
	}
};

//std::string UnityAdapter::UNITY_CORE_COLOR_NAME = "_Color";