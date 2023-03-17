workspace(name = "tf_serving")

# To update TensorFlow to a new revision.
# 1. Update the 'git_commit' args below to include the new git hash.
# 2. Get the sha256 hash of the archive with a command such as...
#    curl -L https://github.com/tensorflow/tensorflow/archive/<git hash>.tar.gz | sha256sum
#    and update the 'sha256' arg with the result.
# 3. Request the new archive to be mirrored on mirror.bazel.build for more
#    reliable downloads.
load("//tensorflow_serving:repo.bzl", "tensorflow_http_archive")

tensorflow_http_archive(
    name = "org_tensorflow",
    sha256 = "044099fa9fb95147a0c9f5671d03d5e3ac4243a11bf4edbc7853952a8ea419e6",
    git_commit = "3126efeca0b73fde4a85aa65ebbd5b5f90a5e5db",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# START: Upstream TensorFlow dependencies
# TensorFlow build depends on these dependencies.
# Needs to be in-sync with TensorFlow sources.
http_archive(
    name = "io_bazel_rules_closure",
    sha256 = "ddce3b3a3909f99b28b25071c40b7fec7e2e1d1d1a4b2e933f3082aa99517105",
    strip_prefix = "rules_closure-316e6133888bfc39fb860a4f1a31cfcbae485aef",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_closure/archive/316e6133888bfc39fb860a4f1a31cfcbae485aef.tar.gz",
        "https://github.com/bazelbuild/rules_closure/archive/316e6133888bfc39fb860a4f1a31cfcbae485aef.tar.gz",  # 2019-03-21
    ],
)
http_archive(
    name = "bazel_skylib",
    sha256 = "2ef429f5d7ce7111263289644d233707dba35e39696377ebab8b0bc701f7818e",
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/0.8.0/bazel-skylib.0.8.0.tar.gz"],
)

http_archive(
    name = "com_github_nelhage_rules_boost",
    sha256 = "f8c9653c1c49489c04f9f87ab1ee93d7b59bb26a39d9e30e9687fca3c6197c3f",
		strip_prefix = "rules_boost-9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
		urls = ["https://github.com/nelhage/rules_boost/archive/9f9fb8b2f0213989247c9d5c0e814a8451d18d7f.tar.gz"],
)

http_archive(
    name = "aliyun_oss_c_sdk",
    build_file = "//third_party/oss_c_sdk:oss_c_sdk.BUILD",
    sha256 = "6450d3970578c794b23e9e1645440c6f42f63be3f82383097660db5cf2fba685",
    strip_prefix = "aliyun-oss-c-sdk-3.7.0",
    urls = [
        "http://pythonrun.oss-cn-zhangjiakou.aliyuncs.com/tensorflow_io/github.com/aliyun/aliyun-oss-c-sdk/archive/3.7.0.tar.gz",
    ],
)

http_archive(
    name = "libexpat",
    build_file = "//third_party/expat:libexpat.BUILD",
    sha256 = "574499cba22a599393e28d99ecfa1e7fc85be7d6651d543045244d5b561cb7ff",
    strip_prefix = "libexpat-R_2_2_6/expat",
    urls = [
        "http://pythonrun.oss-cn-zhangjiakou.aliyuncs.com/tensorflow_io/github.com/libexpat/libexpat/archive/R_2_2_6.tar.gz",
    ],
)

http_archive(
    name = "libapr1",
    build_file = "//third_party/apr1:libapr1.BUILD",
    sha256 = "1a0909a1146a214a6ab9de28902045461901baab4e0ee43797539ec05b6dbae0",
    strip_prefix = "apr-1.6.5",
    patches = [
        "//third_party/apr1:libapr1.patch",
    ],
    urls = [
        "http://pythonrun.oss-cn-zhangjiakou.aliyuncs.com/tensorflow_io/github.com/apache/apr/archive/1.6.5.tar.gz",
    ],
)

http_archive(
    name = "libaprutil1",
    build_file = "//third_party/aprutil1:libaprutil1.BUILD",
    sha256 = "4c9ae319cedc16890fc2776920e7d529672dda9c3a9a9abd53bd80c2071b39af",
    strip_prefix = "apr-util-1.6.1",
    patches = [
        "//third_party/aprutil1:libaprutil1.patch",
    ],
    urls = [
        "http://pythonrun.oss-cn-zhangjiakou.aliyuncs.com/tensorflow_io/github.com/apache/apr-util/archive/1.6.1.tar.gz",
    ],
)

http_archive(
    name = "mxml",
    build_file = "//third_party/mxml:mxml.BUILD",
    sha256 = "4d850d15cdd4fdb9e82817eb069050d7575059a9a2729c82b23440e4445da199",
    strip_prefix = "mxml-2.12",
    patches = [
        "//third_party/mxml:mxml.patch",
    ],
    urls = [
        "http://pythonrun.oss-cn-zhangjiakou.aliyuncs.com/tensorflow_io/github.com/michaelrsweet/mxml/archive/v2.12.tar.gz",
    ],
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

# END: Upstream TensorFlow dependencies

# Please add all new TensorFlow Serving dependencies in workspace.bzl.
load("//tensorflow_serving:workspace.bzl", "tf_serving_workspace")

tf_serving_workspace()

# Specify the minimum required bazel version.
load("@org_tensorflow//tensorflow:version_check.bzl", "check_bazel_version_at_least")

check_bazel_version_at_least("0.24.1")
