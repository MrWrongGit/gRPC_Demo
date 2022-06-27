#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <unistd.h>
#include "stitch.grpc.pb.h"
#include "stitch_server_impl.h"

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;

void *worker(void *arg)
{
    StitchServerImpl *srv_p = (StitchServerImpl *)arg;
    cout << "worker start: project=" << srv_p->prj << " poi=" << srv_p->poi << endl;
    int progress = 0;
    while(progress < 100) {
        progress ++;
        srv_p->updateProgress(progress);
        usleep(50000);
    }
    cout << "worker end" << endl;
}

int main(int argc, char** argv) {
    // grpc server setup
    string server_address("0.0.0.0:50051");
    StitchServerImpl service;
    service.setupWorker(worker);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    // start grpc server
    unique_ptr<Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;
    // grpc running
    server->Wait();
}
