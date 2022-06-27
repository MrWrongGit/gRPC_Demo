#include <unistd.h>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include "stitch.grpc.pb.h"

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;

void startStitching(std::unique_ptr<StitchSrv::Stub> &rpc_stub, string prj, string poi)
{
    ClientContext context;

    StitchArgs stitch_args;
    stitch_args.set_prj(prj);
    stitch_args.set_poi(poi);
    // stream
    std::unique_ptr<ClientReader<Progress> >reader(rpc_stub->start(&context, stitch_args));
    Progress progress;
    while (reader->Read(&progress)) {
        cout << "stitching progress " << progress.progress() << endl;
    }

    Status status = reader->Finish();
    if (status.ok()) {
        cout << "stitching ok!" << endl;
    } else {
        cout << "stitching fail!" << endl;
    }
}

int main(int argc, char** argv)
{
    // creat channel (create socket)
    std::shared_ptr<Channel> rpc_channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    // creat stub (set callback)
    std::unique_ptr<StitchSrv::Stub> rpc_stub = StitchSrv::NewStub(rpc_channel);
    // call start
    startStitching(rpc_stub, "test", "poi_2");

    return 0;
}
