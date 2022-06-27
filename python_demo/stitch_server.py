import grpc
import stitch_pb2_grpc
import stitch_pb2
from concurrent import futures
import time

class StitchServerImpl(stitch_pb2_grpc.StitchSrv):
    def __init__(self):
        pass

    def start(self, requst, context):
        progress = 0
        print('start: project=',requst.prj, ' poi=', requst.poi)
        while progress < 100:
            progress += 1
            yield stitch_pb2.Progress(progress=progress)
            time.sleep(0.05)

server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
stitch_pb2_grpc.add_StitchSrvServicer_to_server(StitchServerImpl(), server)
server.add_insecure_port('[::]:50051')
server.start()
print("start service...")
try:
    while True:
        time.sleep(10)
except KeyboardInterrupt:
    server.stop(0)