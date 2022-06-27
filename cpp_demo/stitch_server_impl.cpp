
#include "stitch.grpc.pb.h"
#include "stitch.pb.h"
#include "stitch_server_impl.h"
#include <unistd.h>

StitchServerImpl::StitchServerImpl()
{
    _worker_p = nullptr;
    _progress = 0;
    _progress_update = false;
    _busy = false;
}

Status StitchServerImpl::cancel(ServerContext* context, const StitchArgs* args, Response* ret)
{

}

Status StitchServerImpl::start(ServerContext* context, const StitchArgs* args, ServerWriter<Progress>* writer)
{
    Progress msg;
    // no worker
    if(_worker_p == nullptr) {
        msg.set_progress(-255);
        writer->Write(msg);
        return Status::CANCELLED;
    }
    // busy
    pthread_mutex_lock(&_busy_mutex);
    if(_busy) {
        pthread_mutex_unlock(&_busy_mutex);
        // busy, reject
        msg.set_progress(-254);
        writer->Write(msg);
        return Status::CANCELLED;
    }
    // set busy
    _busy = true;
    pthread_mutex_unlock(&_busy_mutex);

    // create worker thread
    _progress = 0;
    _progress_update = false;
    prj = args->prj();
    poi = args->poi();
    pthread_create(&_worker_thread, NULL, _worker_p, (void *)this);

    while(1) {
        if(_progress_update) {
            _progress_update = false;
            // sync to client
            msg.set_progress(_progress);
            writer->Write(msg);
        }
        // error or sucess
        if((_progress < 0) || (_progress >= 100)) {
            break;
        }
        usleep(250000);
    }
    // wait for worker thread finish
    pthread_join(_worker_thread, NULL);

    // reset state for new call
    pthread_mutex_lock(&_busy_mutex);
    _busy = false;
    pthread_mutex_unlock(&_busy_mutex);

    return (_progress==100) ? Status::OK : Status::CANCELLED;
}

void StitchServerImpl::updateProgress(int progress)
{
    _progress = progress;
    _progress_update = true;
}

void StitchServerImpl::setupWorker(void *(*worker_p)(void *))
{
    _worker_p = worker_p;
}
