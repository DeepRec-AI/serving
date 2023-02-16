# Description:
#   aliyun_oss_c_sdk

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache 2.0

exports_files(["LICENSE"])

cc_library(
    name = "aliyun_oss_c_sdk",
    srcs = [
        "oss_c_sdk/aos_buf.c",
        "oss_c_sdk/aos_buf.h",
        "oss_c_sdk/aos_crc64.c",
        "oss_c_sdk/aos_crc64.h",
        "oss_c_sdk/aos_fstack.c",
        "oss_c_sdk/aos_fstack.h",
        "oss_c_sdk/aos_http_io.c",
        "oss_c_sdk/aos_http_io.h",
        "oss_c_sdk/aos_list.h",
        "oss_c_sdk/aos_log.c",
        "oss_c_sdk/aos_status.c",
        "oss_c_sdk/aos_string.c",
        "oss_c_sdk/aos_transport.c",
        "oss_c_sdk/aos_transport.h",
        "oss_c_sdk/aos_util.c",
        "oss_c_sdk/oss_auth.c",
        "oss_c_sdk/oss_bucket.c",
        "oss_c_sdk/oss_define.c",
        "oss_c_sdk/oss_define.h",
        "oss_c_sdk/oss_live.c",
        "oss_c_sdk/oss_multipart.c",
        "oss_c_sdk/oss_object.c",
        "oss_c_sdk/oss_resumable.c",
        "oss_c_sdk/oss_resumable.h",
        "oss_c_sdk/oss_util.c",
        "oss_c_sdk/oss_xml.c",
        "oss_c_sdk/oss_xml.h",
    ],
    hdrs = [
        "oss_c_sdk/aos_define.h",
        "oss_c_sdk/aos_log.h",
        "oss_c_sdk/aos_status.h",
        "oss_c_sdk/aos_string.h",
        "oss_c_sdk/aos_util.h",
        "oss_c_sdk/oss_api.h",
        "oss_c_sdk/oss_auth.h",
        "oss_c_sdk/oss_util.h",
    ],
    includes = [
        "oss_c_sdk",
    ],
    deps = [
        "@curl",
        "@libapr1",
        "@libaprutil1",
        "@mxml",
    ],
)
