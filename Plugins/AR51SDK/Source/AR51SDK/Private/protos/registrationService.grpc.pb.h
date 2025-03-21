// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: registrationService.proto
#ifndef GRPC_registrationService_2eproto__INCLUDED
#define GRPC_registrationService_2eproto__INCLUDED

#include "registrationService.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/client_context.h>
#include <grpcpp/impl/codegen/completion_queue.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc_impl {
class CompletionQueue;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc_impl

namespace grpc {
namespace experimental {
template <typename RequestT, typename ResponseT>
class MessageAllocator;
}  // namespace experimental
}  // namespace grpc

namespace AR51 {

// The RegistrationService definition.
class RegistrationService final {
 public:
  static constexpr char const* service_full_name() {
    return "AR51.RegistrationService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    // Provides a notification whenever the listed components in the RegistrationService changes
    std::unique_ptr< ::grpc::ClientReaderInterface< ::AR51::ComponentsChanged>> ListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request) {
      return std::unique_ptr< ::grpc::ClientReaderInterface< ::AR51::ComponentsChanged>>(ListenToComponentsRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>> AsyncListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>>(AsyncListenToComponentsRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>> PrepareAsyncListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>>(PrepareAsyncListenToComponentsRaw(context, request, cq));
    }
    // Provides a list of all registered components (at the moment when the call is made)
    virtual ::grpc::Status GetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::AR51::ComponentList* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>> AsyncGetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>>(AsyncGetComponentsRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>> PrepareAsyncGetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>>(PrepareAsyncGetComponentsRaw(context, request, cq));
    }
    // A client uses this two-way stream call to listen to keep alive requests from the RegistrationService.
    // The client must then respond with a list of its components
    // Closing any of the streams means that the client unregisters, this happens implicitly.
    std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>> Register(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(RegisterRaw(context));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>> AsyncRegister(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(AsyncRegisterRaw(context, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>> PrepareAsyncRegister(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(PrepareAsyncRegisterRaw(context, cq));
    }
    class experimental_async_interface {
     public:
      virtual ~experimental_async_interface() {}
      // Provides a notification whenever the listed components in the RegistrationService changes
      virtual void ListenToComponents(::grpc::ClientContext* context, ::AR51::Empty* request, ::grpc::experimental::ClientReadReactor< ::AR51::ComponentsChanged>* reactor) = 0;
      // Provides a list of all registered components (at the moment when the call is made)
      virtual void GetComponents(::grpc::ClientContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response, std::function<void(::grpc::Status)>) = 0;
      virtual void GetComponents(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::AR51::ComponentList* response, std::function<void(::grpc::Status)>) = 0;
      virtual void GetComponents(::grpc::ClientContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response, ::grpc::experimental::ClientUnaryReactor* reactor) = 0;
      virtual void GetComponents(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::AR51::ComponentList* response, ::grpc::experimental::ClientUnaryReactor* reactor) = 0;
      // A client uses this two-way stream call to listen to keep alive requests from the RegistrationService.
      // The client must then respond with a list of its components
      // Closing any of the streams means that the client unregisters, this happens implicitly.
      virtual void Register(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::AR51::ComponentList,::AR51::IsAliveQuery>* reactor) = 0;
    };
    virtual class experimental_async_interface* experimental_async() { return nullptr; }
  private:
    virtual ::grpc::ClientReaderInterface< ::AR51::ComponentsChanged>* ListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>* AsyncListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderInterface< ::AR51::ComponentsChanged>* PrepareAsyncListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>* AsyncGetComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::AR51::ComponentList>* PrepareAsyncGetComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>* RegisterRaw(::grpc::ClientContext* context) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>* AsyncRegisterRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) = 0;
    virtual ::grpc::ClientAsyncReaderWriterInterface< ::AR51::ComponentList, ::AR51::IsAliveQuery>* PrepareAsyncRegisterRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    std::unique_ptr< ::grpc::ClientReader< ::AR51::ComponentsChanged>> ListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request) {
      return std::unique_ptr< ::grpc::ClientReader< ::AR51::ComponentsChanged>>(ListenToComponentsRaw(context, request));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>> AsyncListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>>(AsyncListenToComponentsRaw(context, request, cq, tag));
    }
    std::unique_ptr< ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>> PrepareAsyncListenToComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>>(PrepareAsyncListenToComponentsRaw(context, request, cq));
    }
    ::grpc::Status GetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::AR51::ComponentList* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>> AsyncGetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>>(AsyncGetComponentsRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>> PrepareAsyncGetComponents(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>>(PrepareAsyncGetComponentsRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>> Register(::grpc::ClientContext* context) {
      return std::unique_ptr< ::grpc::ClientReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(RegisterRaw(context));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>> AsyncRegister(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(AsyncRegisterRaw(context, cq, tag));
    }
    std::unique_ptr<  ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>> PrepareAsyncRegister(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>>(PrepareAsyncRegisterRaw(context, cq));
    }
    class experimental_async final :
      public StubInterface::experimental_async_interface {
     public:
      void ListenToComponents(::grpc::ClientContext* context, ::AR51::Empty* request, ::grpc::experimental::ClientReadReactor< ::AR51::ComponentsChanged>* reactor) override;
      void GetComponents(::grpc::ClientContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response, std::function<void(::grpc::Status)>) override;
      void GetComponents(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::AR51::ComponentList* response, std::function<void(::grpc::Status)>) override;
      void GetComponents(::grpc::ClientContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response, ::grpc::experimental::ClientUnaryReactor* reactor) override;
      void GetComponents(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::AR51::ComponentList* response, ::grpc::experimental::ClientUnaryReactor* reactor) override;
      void Register(::grpc::ClientContext* context, ::grpc::experimental::ClientBidiReactor< ::AR51::ComponentList,::AR51::IsAliveQuery>* reactor) override;
     private:
      friend class Stub;
      explicit experimental_async(Stub* stub): stub_(stub) { }
      Stub* stub() { return stub_; }
      Stub* stub_;
    };
    class experimental_async_interface* experimental_async() override { return &async_stub_; }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    class experimental_async async_stub_{this};
    ::grpc::ClientReader< ::AR51::ComponentsChanged>* ListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request) override;
    ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>* AsyncListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReader< ::AR51::ComponentsChanged>* PrepareAsyncListenToComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>* AsyncGetComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::AR51::ComponentList>* PrepareAsyncGetComponentsRaw(::grpc::ClientContext* context, const ::AR51::Empty& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>* RegisterRaw(::grpc::ClientContext* context) override;
    ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>* AsyncRegisterRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) override;
    ::grpc::ClientAsyncReaderWriter< ::AR51::ComponentList, ::AR51::IsAliveQuery>* PrepareAsyncRegisterRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_ListenToComponents_;
    const ::grpc::internal::RpcMethod rpcmethod_GetComponents_;
    const ::grpc::internal::RpcMethod rpcmethod_Register_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    // Provides a notification whenever the listed components in the RegistrationService changes
    virtual ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer);
    // Provides a list of all registered components (at the moment when the call is made)
    virtual ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response);
    // A client uses this two-way stream call to listen to keep alive requests from the RegistrationService.
    // The client must then respond with a list of its components
    // Closing any of the streams means that the client unregisters, this happens implicitly.
    virtual ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream);
  };
  template <class BaseClass>
  class WithAsyncMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_ListenToComponents() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestListenToComponents(::grpc::ServerContext* context, ::AR51::Empty* request, ::grpc::ServerAsyncWriter< ::AR51::ComponentsChanged>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(0, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_GetComponents() {
      ::grpc::Service::MarkMethodAsync(1);
    }
    ~WithAsyncMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetComponents(::grpc::ServerContext* context, ::AR51::Empty* request, ::grpc::ServerAsyncResponseWriter< ::AR51::ComponentList>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithAsyncMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_Register() {
      ::grpc::Service::MarkMethodAsync(2);
    }
    ~WithAsyncMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegister(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(2, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_ListenToComponents<WithAsyncMethod_GetComponents<WithAsyncMethod_Register<Service > > > AsyncService;
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_ListenToComponents() {
      ::grpc::Service::experimental().MarkMethodCallback(0,
        new ::grpc_impl::internal::CallbackServerStreamingHandler< ::AR51::Empty, ::AR51::ComponentsChanged>(
          [this] { return this->ListenToComponents(); }));
    }
    ~ExperimentalWithCallbackMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::AR51::Empty, ::AR51::ComponentsChanged>* ListenToComponents() {
      return new ::grpc_impl::internal::UnimplementedWriteReactor<
        ::AR51::Empty, ::AR51::ComponentsChanged>;}
  };
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_GetComponents() {
      ::grpc::Service::experimental().MarkMethodCallback(1,
        new ::grpc_impl::internal::CallbackUnaryHandler< ::AR51::Empty, ::AR51::ComponentList>(
          [this](::grpc::ServerContext* context,
                 const ::AR51::Empty* request,
                 ::AR51::ComponentList* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   return this->GetComponents(context, request, response, controller);
                 }));
    }
    void SetMessageAllocatorFor_GetComponents(
        ::grpc::experimental::MessageAllocator< ::AR51::Empty, ::AR51::ComponentList>* allocator) {
      static_cast<::grpc_impl::internal::CallbackUnaryHandler< ::AR51::Empty, ::AR51::ComponentList>*>(
          ::grpc::Service::experimental().GetHandler(1))
              ->SetMessageAllocator(allocator);
    }
    ~ExperimentalWithCallbackMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class ExperimentalWithCallbackMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithCallbackMethod_Register() {
      ::grpc::Service::experimental().MarkMethodCallback(2,
        new ::grpc_impl::internal::CallbackBidiHandler< ::AR51::ComponentList, ::AR51::IsAliveQuery>(
          [this] { return this->Register(); }));
    }
    ~ExperimentalWithCallbackMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerBidiReactor< ::AR51::ComponentList, ::AR51::IsAliveQuery>* Register() {
      return new ::grpc_impl::internal::UnimplementedBidiReactor<
        ::AR51::ComponentList, ::AR51::IsAliveQuery>;}
  };
  typedef ExperimentalWithCallbackMethod_ListenToComponents<ExperimentalWithCallbackMethod_GetComponents<ExperimentalWithCallbackMethod_Register<Service > > > ExperimentalCallbackService;
  template <class BaseClass>
  class WithGenericMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_ListenToComponents() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_GetComponents() {
      ::grpc::Service::MarkMethodGeneric(1);
    }
    ~WithGenericMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithGenericMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_Register() {
      ::grpc::Service::MarkMethodGeneric(2);
    }
    ~WithGenericMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_ListenToComponents() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestListenToComponents(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncWriter< ::grpc::ByteBuffer>* writer, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncServerStreaming(0, context, request, writer, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_GetComponents() {
      ::grpc::Service::MarkMethodRaw(1);
    }
    ~WithRawMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestGetComponents(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(1, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithRawMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_Register() {
      ::grpc::Service::MarkMethodRaw(2);
    }
    ~WithRawMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestRegister(::grpc::ServerContext* context, ::grpc::ServerAsyncReaderWriter< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* stream, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncBidiStreaming(2, context, stream, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_ListenToComponents() {
      ::grpc::Service::experimental().MarkMethodRawCallback(0,
        new ::grpc_impl::internal::CallbackServerStreamingHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this] { return this->ListenToComponents(); }));
    }
    ~ExperimentalWithRawCallbackMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerWriteReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* ListenToComponents() {
      return new ::grpc_impl::internal::UnimplementedWriteReactor<
        ::grpc::ByteBuffer, ::grpc::ByteBuffer>;}
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_GetComponents() {
      ::grpc::Service::experimental().MarkMethodRawCallback(1,
        new ::grpc_impl::internal::CallbackUnaryHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this](::grpc::ServerContext* context,
                 const ::grpc::ByteBuffer* request,
                 ::grpc::ByteBuffer* response,
                 ::grpc::experimental::ServerCallbackRpcController* controller) {
                   this->GetComponents(context, request, response, controller);
                 }));
    }
    ~ExperimentalWithRawCallbackMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual void GetComponents(::grpc::ServerContext* context, const ::grpc::ByteBuffer* request, ::grpc::ByteBuffer* response, ::grpc::experimental::ServerCallbackRpcController* controller) { controller->Finish(::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "")); }
  };
  template <class BaseClass>
  class ExperimentalWithRawCallbackMethod_Register : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    ExperimentalWithRawCallbackMethod_Register() {
      ::grpc::Service::experimental().MarkMethodRawCallback(2,
        new ::grpc_impl::internal::CallbackBidiHandler< ::grpc::ByteBuffer, ::grpc::ByteBuffer>(
          [this] { return this->Register(); }));
    }
    ~ExperimentalWithRawCallbackMethod_Register() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status Register(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::AR51::IsAliveQuery, ::AR51::ComponentList>* stream)  override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    virtual ::grpc::experimental::ServerBidiReactor< ::grpc::ByteBuffer, ::grpc::ByteBuffer>* Register() {
      return new ::grpc_impl::internal::UnimplementedBidiReactor<
        ::grpc::ByteBuffer, ::grpc::ByteBuffer>;}
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_GetComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_GetComponents() {
      ::grpc::Service::MarkMethodStreamed(1,
        new ::grpc::internal::StreamedUnaryHandler< ::AR51::Empty, ::AR51::ComponentList>(std::bind(&WithStreamedUnaryMethod_GetComponents<BaseClass>::StreamedGetComponents, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_GetComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status GetComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::AR51::ComponentList* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedGetComponents(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::AR51::Empty,::AR51::ComponentList>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_GetComponents<Service > StreamedUnaryService;
  template <class BaseClass>
  class WithSplitStreamingMethod_ListenToComponents : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithSplitStreamingMethod_ListenToComponents() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::SplitServerStreamingHandler< ::AR51::Empty, ::AR51::ComponentsChanged>(std::bind(&WithSplitStreamingMethod_ListenToComponents<BaseClass>::StreamedListenToComponents, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithSplitStreamingMethod_ListenToComponents() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status ListenToComponents(::grpc::ServerContext* context, const ::AR51::Empty* request, ::grpc::ServerWriter< ::AR51::ComponentsChanged>* writer) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with split streamed
    virtual ::grpc::Status StreamedListenToComponents(::grpc::ServerContext* context, ::grpc::ServerSplitStreamer< ::AR51::Empty,::AR51::ComponentsChanged>* server_split_streamer) = 0;
  };
  typedef WithSplitStreamingMethod_ListenToComponents<Service > SplitStreamedService;
  typedef WithSplitStreamingMethod_ListenToComponents<WithStreamedUnaryMethod_GetComponents<Service > > StreamedService;
};

}  // namespace AR51


#endif  // GRPC_registrationService_2eproto__INCLUDED
