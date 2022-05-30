/* Copyright 2016 Google Inc. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow_serving/servables/tensorflow/saved_model_bundle_v2_factory.h"

#include "absl/strings/string_view.h"
#include "tensorflow/cc/saved_model/tag_constants.h"
#include "tensorflow/contrib/session_bundle/bundle_shim.h"
#include "tensorflow/core/framework/tensor.pb.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/protobuf/config.pb.h"
#include "tensorflow/core/protobuf/meta_graph.pb.h"
#include "tensorflow/core/protobuf/named_tensor.pb.h"
#include "tensorflow/core/public/session_options.h"
#include "tensorflow_serving/servables/tensorflow/bundle_factory_util.h"
#include "tensorflow_serving/servables/tensorflow/curried_session.h"
#include "tensorflow_serving/servables/tensorflow/tflite_session.h"

namespace tensorflow {
namespace serving {

namespace {

// Extracts the signatures from 'bundle'.
std::vector<SignatureDef> GetSignatureDefs(const SavedModelBundleV2& bundle) {
  std::vector<SignatureDef> signature_defs;
  for (const auto& entry : bundle.meta_graph_def.signature_def()) {
    const SignatureDef& signature_def = entry.second;
    signature_defs.push_back(signature_def);
  }
  return signature_defs;
}

// Parses a repeated field of NamedTensorProtos into a corresponding list of
// name/tensor pairs.
Status ParseFixedInputTensors(
    const protobuf::RepeatedPtrField<NamedTensorProto>& protos,
    std::vector<std::pair<string, Tensor>>* parsed) {
  for (const NamedTensorProto& proto : protos) {
    Tensor tensor;
    if (!tensor.FromProto(proto.tensor())) {
      return errors::InvalidArgument("Unable to parse tensor proto: ",
                                     proto.tensor().ShortDebugString());
    }
    parsed->push_back({proto.name(), tensor});
  }
  return Status::OK();
}

// TODO(b/140959776): Move this upstream alongside `kSavedModelFilenamePb`.
const char kTfLiteModelFilename[] = "model.tflite";

Status LoadTfLiteModel(const string& model_dir, SavedModelBundleV2* bundle) {
  LOG(FATAL) << "Not support tflite.";
  return Status::OK();
}

}  // namespace

Status SavedModelBundleV2Factory::Create(
    const SessionGroupBundleConfig& config,
    std::unique_ptr<SavedModelBundleV2Factory>* factory) {
  std::shared_ptr<Batcher> batcher;
  if (config.has_batching_parameters()) {
    TF_RETURN_IF_ERROR(
        CreateBatchScheduler(config.batching_parameters(), &batcher));
  }
  factory->reset(new SavedModelBundleV2Factory(config, batcher));
  return Status::OK();
}

Status SavedModelBundleV2Factory::EstimateResourceRequirement(
    const string& path, ResourceAllocation* estimate) const {
  return EstimateResourceFromPath(path, estimate);
}

Status SavedModelBundleV2Factory::CreateSavedModelBundleV2WithMetadata(
    const Loader::Metadata& metadata, const string& path,
    std::unique_ptr<SavedModelBundleV2>* bundle) {
  return InternalCreateSavedModelBundleV2(metadata, path, bundle);
}

Status SavedModelBundleV2Factory::CreateSavedModelBundleV2(
    const string& path, std::unique_ptr<SavedModelBundleV2>* bundle) {
  return InternalCreateSavedModelBundleV2({}, path, bundle);
}

Status SavedModelBundleV2Factory::InternalCreateSavedModelBundleV2(
    const absl::optional<Loader::Metadata>& metadata, const string& path,
    std::unique_ptr<SavedModelBundleV2>* bundle) {
  bundle->reset(new SavedModelBundleV2);
  std::unordered_set<string> saved_model_tags(
      config_.saved_model_tags().begin(), config_.saved_model_tags().end());
  // Defaults to loading the meta graph def corresponding to the `serve` tag if
  // no `saved_model_tags` are specified.
  if (saved_model_tags.empty()) {
    saved_model_tags.insert(kSavedModelTagServe);
  }
  const auto& session_options = [&]() {
    auto result = GetSessionOptions(config_);
    if (metadata.has_value()) {
      auto* session_metadata =
          result.config.mutable_experimental()->mutable_session_metadata();
      session_metadata->set_name(metadata->servable_id.name);
      session_metadata->set_version(metadata->servable_id.version);
    }
    return result;
  }();

  if (config_.use_tflite_model()) {
    // TODO: FIXME TF_RETURN_IF_ERROR(LoadTfLiteModel(path, bundle->get()));
    LOG(FATAL) << "No support tflite.";
  } else {
    TF_RETURN_IF_ERROR(LoadSessionBundleOrSavedModelBundle(
        session_options, GetRunOptions(config_), path, saved_model_tags,
        bundle->get()));
  }
  if (!config_.experimental_fixed_input_tensors().empty()) {
    LOG(FATAL) << "Not support fixed input tensors.";
  }
  if (config_.remove_unused_fields_from_bundle_metagraph()) {
    // Save memory by removing fields in MetaGraphDef proto message stored
    // in the bundle that we never use. Notably the unused graphdef submessage
    // can get large (MBs) wasting memory on the server.
    //
    // Presently we retain following field(s) of MetaGraphDef proto:
    // - signature_def
    MetaGraphDef metagraph;
    (*bundle)->meta_graph_def.Swap(&metagraph);
    (*bundle)->meta_graph_def.mutable_signature_def()->swap(
        *metagraph.mutable_signature_def());
  }
  if (config_.has_batching_parameters()) {
    LOG(INFO) << "Wrapping session to perform batch processing";
    if (batch_scheduler_ == nullptr) {
      return errors::Internal("batch_scheduler_ not set");
    }
    // Enable batching of requests to any one signature_def in the SavedModel.
    // Note that in the future, the plan is to enable explicit configuration of
    // the one or many SignatureDefs to enable.
    const std::vector<SignatureDef> signatures = GetSignatureDefs(**bundle);
    return WrapSessionGroupForBatching(config_.batching_parameters(),
                                       batch_scheduler_, signatures,
                                       &(*bundle)->session_group);
  }
  return Status::OK();
}

SavedModelBundleV2Factory::SavedModelBundleV2Factory(
    const SessionGroupBundleConfig& config, std::shared_ptr<Batcher> batch_scheduler)
    : config_(config), batch_scheduler_(batch_scheduler) {}

}  // namespace serving
}  // namespace tensorflow
