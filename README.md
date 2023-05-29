# DeepRec Serving

DeepRec Serving is a high-performance serving system for DeepRec based on TensorFlow Serving.
DeepRec Serving could highly improve performance and cpu/gpu utilization in inference, such as SessionGroup, CUDA multi-stream, etc.

Few features in DeepRec Serving:
-   Support SessionGroup which is shared-variable (only variables shared) architecture for mutliple session in serving process.
-   Support CUDA Multiple Stream, could highly improve QPS and GPU Utilization in GPU Inference.

## Installation

### Prepare for build

**CPU Dev Docker**

| GCC Version | Python Version |                           IMAGE                           |
| ----------- | -------------- | --------------------------------------------------------- |
|   9.4.0     |    3.8.10      | alideeprec/deeprec-build:deeprec-dev-cpu-py38-ubuntu20.04 |

**GPU(cuda11.6) Dev Docker**

| GCC Version | Python Version | CUDA VERSION |                           IMAGE                                 |
| ----------- | -------------- | ------------ | --------------------------------------------------------------- |
|    9.4.0    |    3.8.10      | CUDA 11.6.2  | alideeprec/deeprec-build:deeprec-dev-gpu-py38-cu116-ubuntu20.04 |

### Build from source

Develop Branch: master, Latest Release Branch: deeprec2302

**Build Package Builder-CPU**

```bash
bazel build -c opt tensorflow_serving/...
```

**Build CPU Package Builder with OneDNN + Eigen Threadpool**

```bash
bazel build  -c opt --config=mkl_threadpool --define build_with_mkl_dnn_v1_only=true tensorflow_serving/...
```

**Build Package Builder-GPU**

```bash
bazel build -c opt --config=cuda tensorflow_serving/...
```

**Build Package**

```bash
bazel-bin/tensorflow_serving/tools/pip_package/build_pip_package /tmp/tf_serving_client_whl
```

**Server Bin**

Server Bin would generated in following directory:
```bash
bazel-bin/tensorflow_serving/model_servers/tensorflow_model_server
```
## More details 

* [SessionGroup](https://github.com/DeepRec-AI/DeepRec/blob/main/docs/docs_en/SessionGroup.md)
* [CUDA MultiStream](https://github.com/DeepRec-AI/DeepRec/blob/main/docs/docs_en/GPU-MultiStream.md)
* [Device Placement Optimization](https://github.com/DeepRec-AI/DeepRec/blob/main/docs/docs_en/Device-Placement.md)
