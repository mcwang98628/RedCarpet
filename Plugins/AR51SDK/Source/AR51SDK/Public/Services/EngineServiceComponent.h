#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"

#include "EngineServiceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UEngineServiceComponent : public UActorComponent, public IServiceComponent<UEngineServiceComponent>, public AR51::UnityService::Service
{
	GENERATED_BODY()
        
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TArray<TSubclassOf<AActor>> InstanceBlueprints{};
	UEngineServiceComponent();

private:
    double _pyTimeOffset{ 0 };
    TArray<AActor*> _instances{};

    bool TryGetTransform(int64_t id, AR51::TransformTypes type, std::string& outBytestring) const;

    grpc::Status ActorMissing(const std::string& method, int64_t id);

public:
    grpc::Status GetPyTime(grpc::ServerContext* context, const AR51::Empty* request, AR51::Float64Message* response) override;
    grpc::Status GetPyTimeOffset(grpc::ServerContext* context, const AR51::Empty* request, AR51::Float64Message* response) override;
    grpc::Status SetPyTimeOffset(grpc::ServerContext* context, const AR51::Float64Message* request, AR51::Empty* response) override;
    grpc::Status Instantiate(grpc::ServerContext* context, const AR51::InstantiateRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status FindGameObject(grpc::ServerContext* context, const AR51::NameRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status FindAnchorGameObject(grpc::ServerContext* context, const AR51::AnchorRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status GetMainCamera(grpc::ServerContext* context, const AR51::Empty* request, AR51::ObjectIdMessage* response) override;
    grpc::Status Destroy(grpc::ServerContext* context, const AR51::ObjectIdMessage* request, AR51::Empty* response) override;
    grpc::Status CreatePrimitive(grpc::ServerContext* context, const AR51::CreatePrimitiveRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status NewGameObject(grpc::ServerContext* context, const AR51::NameRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status ClearInstances(grpc::ServerContext* context, const AR51::Empty* request, AR51::Empty* response) override;
    grpc::Status SetParent(grpc::ServerContext* context, const AR51::SetParentRequest* request, AR51::Empty* response) override;
    grpc::Status GetName(grpc::ServerContext* context, const AR51::ObjectIdMessage* request, AR51::NameRequest* response) override;
    grpc::Status SetName(grpc::ServerContext* context, const AR51::SetNameRequest* request, AR51::Empty* response) override;
    grpc::Status GetActive(grpc::ServerContext* context, const AR51::ObjectIdMessage* request, AR51::ActiveMessage* response) override;
    grpc::Status SetActive(grpc::ServerContext* context, const AR51::ActiveMessage* request, AR51::ActiveMessage* response) override;
    grpc::Status GetTransform(grpc::ServerContext* context, const AR51::GetTransformRequest* request, AR51::TransformMessage* response) override;
    grpc::Status SetTransform(grpc::ServerContext* context, const AR51::SetTransformRequest* request, AR51::Empty* response) override;
    grpc::Status StreamTransform(grpc::ServerContext* context, const AR51::GetTransformRequest* request, grpc::ServerWriter< AR51::TransformMessage>* writer) override;
    grpc::Status GetComponent(grpc::ServerContext* context, const AR51::ComponentRequest* request, AR51::ObjectIdMessage* response) override;
    grpc::Status GetComponentField(grpc::ServerContext* context, const AR51::ComponentFieldMessage* request, AR51::ComponentFieldMessage* response) override;
    grpc::Status SetComponentField(grpc::ServerContext* context, const AR51::ComponentFieldMessage* request, AR51::Empty* response) override;
    grpc::Status SetCameraProject(grpc::ServerContext* context, const AR51::TransformMessage* request, AR51::Empty* response) override;
    grpc::Status SetRendererEnabled(grpc::ServerContext* context, const AR51::SetRendererEnabledRequest* request, AR51::Empty* response) override;
    grpc::Status SetMaterialTexture(grpc::ServerContext* context, const AR51::SetMaterialTextureRequest* request, AR51::Empty* response) override;
    grpc::Status SetMaterialInt32(grpc::ServerContext* context, const AR51::SetMaterialInt32Request* request, AR51::Empty* response) override;
    grpc::Status SetMaterialFloat(grpc::ServerContext* context, const AR51::SetMaterialFloatRequest* request, AR51::Empty* response) override;
    grpc::Status SetMaterialColor(grpc::ServerContext* context, const AR51::SetMaterialColorRequest* request, AR51::Empty* response) override;
    grpc::Status GetMaterialColor(grpc::ServerContext* context, const AR51::GetMaterialColorRequest* request, AR51::Color* response) override;

    using UActorComponent::SetActive;
protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
