#ifndef __STITCH_SERVER_IMPL_H__
#define __STITCH_SERVER_IMPL_H__

#include <grpc/grpc.h>
#include <pthread.h>
#include "stitch.grpc.pb.h"

using namespace std;

using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

class StitchServerImpl final : public StitchSrv::Service {
public:
    StitchServerImpl();

    // override
    Status start(ServerContext* context, const StitchArgs* args, ServerWriter<Progress>* writer);
    Status cancel(ServerContext* context, const StitchArgs* args, Response* ret);

    void updateProgress(int progress);
    void setupWorker(void *(*)(void *));

    std::string prj, poi;
private:
    void *(*_worker_p)(void *);
    pthread_t _worker_thread;

    pthread_mutex_t _busy_mutex;
    bool _busy;

    int _progress;
    bool _progress_update;
};

#endif