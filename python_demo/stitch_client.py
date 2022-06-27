import grpc
import stitch_pb2_grpc
import stitch_pb2

ch = grpc.insecure_channel('localhost:50051')
srv = stitch_pb2_grpc.StitchSrvStub(channel=ch)

response = srv.start(stitch_pb2.StitchArgs(prj="test", poi="poi_2"))
result = True
try:
    for r in response:
        print('stitching progress ', r.progress)
except:
    result = False
    print('stitching fail!')
if result:
    print('stitching ok!')
