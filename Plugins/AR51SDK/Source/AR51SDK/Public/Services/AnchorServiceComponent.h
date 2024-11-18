#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServiceComponent.h"
#include "Materials/Material.h"
#include "AnchorServiceComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AR51SDK_API UAnchorServiceComponent : public UActorComponent, public IServiceComponent<UAnchorServiceComponent>, public AR51::AnchorService::Service
{
	GENERATED_BODY()

public:
    struct Boundary {
        static Boundary Empty;
        FString Id;
        AActor* Actor;
        TArray<FVector> Points;
        Boundary(const FString& id, AActor* actor, const TArray<FVector>& points);
    };
    TMap<FString, AActor*> _anchorById;
    TMap<FString, Boundary> _boundaryById;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    bool IsAutoSyncCameraToAnchor { true };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TSubclassOf<AActor> AnchorBP = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TSubclassOf<AActor> BoundaryBP = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    TSubclassOf<UMaterial> BoundaryMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
    float BoundaryHeight{100.f};

    const TArray<AActor*> GetAnchors() const;
    AActor* GetAnchorById(const std::string& id) const;
    AActor* GetAnchorById(const FString& id) const;

    static void AnchorToWorldSpace(FVector& position);
    static void AnchorToWorldSpace(TArray<FVector>& positions);
    static void AnchorToWorldSpace(FQuat& rotation);

    static void WorldToAnchorSpace(FVector& position);
    static void WorldToAnchorSpace(TArray<FVector>& positions);
    static void WorldToAnchorSpace(FQuat& rotation);
    static void WorldToAnchorSpace(TArray<FQuat>& rotations);

    static FTransform GetAnchorTransform();

private:
    Boundary CreateBoundary(const FString& id, const TArray<FVector>& points);
    void RefreshNativeBoundary();
    const TArray<Boundary> GetBoundaries() const;
    const TArray<FVector> GetBoundaryPoints(const FString& id);
    Boundary FindBoundary(const std::string& id);
    Boundary FindBoundary(const FString& id);

public:
	UAnchorServiceComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


    ::grpc::Status CreateAnchor(::grpc::ServerContext* context, const ::AR51::CreateAnchorRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status DeleteAnchor(::grpc::ServerContext* context, const ::AR51::AnchorRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status GetAnchorVisibility(::grpc::ServerContext* context, const ::AR51::AnchorRequest* request, ::AR51::BoolMessage* response) override;
    ::grpc::Status SetAnchorVisibility(::grpc::ServerContext* context, const ::AR51::SetAnchorVisiliblityRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status TapToPlaceAnchor(::grpc::ServerContext* context, const ::AR51::AnchorRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status DownloadAnchor(::grpc::ServerContext* context, const ::AR51::AnchorRequest* request, ::grpc::ServerWriter< ::AR51::AnchorBytes>* writer) override;
    ::grpc::Status UploadAnchor(::grpc::ServerContext* context, ::grpc::ServerReader< ::AR51::AnchorBytes>* reader, ::AR51::Empty* response) override;
    ::grpc::Status GetAnchorIds(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::AnchorIdsReply* response) override;
    ::grpc::Status SetAllAnchorVisibility(::grpc::ServerContext* context, const ::AR51::BoolMessage* request, ::AR51::Empty* response) override;
    ::grpc::Status Clear(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::Empty* response) override;

    // ---- Guardians ----
    ::grpc::Status GetGuardian(::grpc::ServerContext* context, const ::AR51::GuardianRequest* request, ::AR51::GetGuardianResponse* response) override;
    ::grpc::Status GetGuardianIds(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::GetGuardianIdsResponse* response) override;
    ::grpc::Status CreateGuardian(::grpc::ServerContext* context, const ::AR51::CreateGuardianRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status DestroyGuardian(::grpc::ServerContext* context, const ::AR51::GuardianRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status ClearGuardians(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::Empty* response) override;
    ::grpc::Status ShowGuardian(::grpc::ServerContext* context, const ::AR51::GuardianRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status HideGuardian(::grpc::ServerContext* context, const ::AR51::GuardianRequest* request, ::AR51::Empty* response) override;
    ::grpc::Status ShowAllGuardians(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::Empty* response) override;
    ::grpc::Status HideAllGuardians(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::Empty* response) override;
};
