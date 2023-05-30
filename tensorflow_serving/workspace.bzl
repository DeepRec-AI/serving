# TensorFlow Serving external dependencies that can be loaded in WORKSPACE
# files.

load("@org_tensorflow//third_party:repo.bzl", "tf_http_archive")
load("@org_tensorflow//tensorflow:workspace.bzl", "tf_workspace")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def tf_serving_workspace():
    """All TensorFlow Serving external dependencies."""

    tf_workspace(path_prefix = "", tf_repo_name = "org_tensorflow")

    # ===== gRPC dependencies =====
    native.bind(
        name = "libssl",
        actual = "@boringssl//:ssl",
    )

    # gRPC wants the existence of a cares dependence but its contents are not
    # actually important since we have set GRPC_ARES=0 in tools/bazel.rc
    native.bind(
        name = "cares",
        actual = "@grpc//third_party/nanopb:nanopb",
    )

    # ===== RapidJSON (rapidjson.org) dependencies =====
    http_archive(
        name = "com_github_tencent_rapidjson",
        urls = [
            "https://github.com/Tencent/rapidjson/archive/v1.1.0.zip",
        ],
        sha256 = "8e00c38829d6785a2dfb951bb87c6974fa07dfe488aa5b25deec4b8bc0f6a3ab",
        strip_prefix = "rapidjson-1.1.0",
        build_file = "@//third_party/rapidjson:BUILD",
    )

    # ===== libevent (libevent.org) dependencies =====
    http_archive(
        name = "com_github_libevent_libevent",
        urls = [
            "https://github.com/libevent/libevent/archive/release-2.1.8-stable.zip",
        ],
        sha256 = "70158101eab7ed44fd9cc34e7f247b3cae91a8e4490745d9d6eb7edc184e4d96",
        strip_prefix = "libevent-release-2.1.8-stable",
        build_file = "@//third_party/libevent:BUILD",
    )

    # ===== Override TF defined `com_google_absl` (we need a recent version).
    tf_http_archive(
        name = "com_google_absl",
        patch_file = [str(Label("@org_tensorflow//third_party:0001-abseil.patch"))],
        build_file = str(Label("@org_tensorflow//third_party:com_google_absl.BUILD")),
        sha256 = "b6aa25c8283cca9de282bb7f5880b04492af76213b2f48c135c4963c6333a21e",
        strip_prefix = "abseil-cpp-36d37ab992038f52276ca66b9da80c1cf0f57dc2",
        urls = [
            "http://mirror.tensorflow.org/github.com/abseil/abseil-cpp/archive/36d37ab992038f52276ca66b9da80c1cf0f57dc2.tar.gz",
            "https://github.com/abseil/abseil-cpp/archive/36d37ab992038f52276ca66b9da80c1cf0f57dc2.tar.gz",
        ],
    )

    http_archive(
        name = "aws_c_common",
        build_file = str(Label("//third_party/aws_util:aws_c_common.BUILD")),
        sha256 = "e9462a141b5db30006704f537d19b92357a59be38d590272e6118976b0356ccd",
        strip_prefix = "aws-c-common-0.7.4",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/awslabs/aws-c-common/archive/refs/tags/v0.7.4.tar.gz",
            "https://github.com/awslabs/aws-c-common/archive/refs/tags/v0.7.4.tar.gz",
        ],
    )

    http_archive(
        name = "aws_c_io",
        build_file = str(Label("//third_party/aws_util:aws_c_io.BUILD")),
        sha256 = "b60270d23b6e2f4a5d80e64ca6538ba114cd6044b53752964c940f87e59bf0d9",
        strip_prefix = "aws-c-io-0.11.2",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/awslabs/aws-c-io/archive/refs/tags/v0.11.2.tar.gz",
            "https://github.com/awslabs/aws-c-io/archive/refs/tags/v0.11.2.tar.gz",
        ],
    )

    http_archive(
        name = "aws_c_event_stream",
        build_file = str(Label("//third_party/aws_util:aws_c_event_stream.BUILD")),
        sha256 = "bae0c762b6a4b779a0db0f4730512da6cb500e76681ffdcb9f7286d8e26e547a",
        strip_prefix = "aws-c-event-stream-0.2.6",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/awslabs/aws-c-event-stream/archive/refs/tags/v0.2.6.tar.gz",
            "https://github.com/awslabs/aws-c-event-stream/archive/refs/tags/v0.2.6.tar.gz",
        ],
    )

    http_archive(
        name = "aws_checksums",
        build_file = str(Label("//third_party/aws_util:aws_checksums.BUILD")),
        sha256 = "394723034b81cc7cd528401775bc7aca2b12c7471c92350c80a0e2fb9d2909fe",
        strip_prefix = "aws-checksums-0.1.12",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/awslabs/aws-checksums/archive/refs/tags/v0.1.12.tar.gz",
            "https://github.com/awslabs/aws-checksums/archive/refs/tags/v0.1.12.tar.gz",
        ],
    )

    http_archive(
        name = "aws_c_cal",
        build_file = str(Label("//third_party/aws_util:aws_c_cal.BUILD")),
        sha256 = "40297da04443d4ee2988d1c5fb0dc4a156d0e4cfaf80e6a1df1867452566d540",
        strip_prefix = "aws-c-cal-0.5.17",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/awslabs/aws-c-cal/archive/refs/tags/v0.5.17.tar.gz",
            "https://github.com/awslabs/aws-c-cal/archive/refs/tags/v0.5.17.tar.gz",
        ],
    )

