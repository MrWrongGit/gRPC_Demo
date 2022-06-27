## gRPC Demo (c++ & python)

### 环境搭建

python

```python
pip install grpcio grpcio-tools protobuf
```

### Demo说明

​	服务端提供stitch(图像拼接)接口，客户端调用start()方法时，服务端会创建woker线程执行stitching算法，并通过gRPC stream特性不断将进度回传至客户端。

### 目录说明

cpp_demo: c++ gRPC demo, 包含服务端及客户端；

cpp_demo/bin: 可执行程序，包含client和server；

python_demo: python gRPC demo, 包含服务端及客户端, 功能和cpp_demo是一样的；

python_demo/gen.sh: proto文件转换为.py文件的指令；

proto: 包含protobuf文件。

