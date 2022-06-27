project(stitch_server)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${stitch_server_SOURCE_DIR}/bin)

########### Protobuf
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

########### gRPC
find_package(Threads REQUIRED)

# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

########### Proto file
get_filename_component(proto_file "../proto/stitch.proto" ABSOLUTE)
get_filename_component(proto_file_path "${proto_file}" PATH)

# Generated sources
set(proto_rpc_file_srcs "${CMAKE_CURRENT_BINARY_DIR}/stitch.pb.cc")
set(proto_rpc_file_hdrs "${CMAKE_CURRENT_BINARY_DIR}/stitch.pb.h")
set(proto_grpc_file_srcs "${CMAKE_CURRENT_BINARY_DIR}/stitch.grpc.pb.cc")
set(proto_grpc_file_hdrs "${CMAKE_CURRENT_BINARY_DIR}/stitch.grpc.pb.h")
add_custom_command(
      OUTPUT "${proto_rpc_file_srcs}" "${proto_rpc_file_hdrs}" "${proto_grpc_file_srcs}" "${proto_grpc_file_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${proto_file_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto_file}"
      DEPENDS "${proto_file}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories(include/)

########### execute
add_executable(stitch_server 
  server.cpp
  stitch_server_impl.cpp
  ${proto_rpc_file_srcs}
  ${proto_grpc_file_srcs}
)
target_link_libraries(stitch_server
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)

add_executable(stitch_client 
  client.cpp
  ${proto_rpc_file_srcs}
  ${proto_grpc_file_srcs}
)
target_link_libraries(stitch_client
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
)