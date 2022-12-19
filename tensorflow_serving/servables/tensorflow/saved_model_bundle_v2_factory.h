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

#ifndef TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_FACTORY_H_
#define TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_FACTORY_H_

#include "absl/types/optional.h"
#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/core/kernels/batching_util/shared_batch_scheduler.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow_serving/batching/batching_session.h"
#include "tensorflow_serving/core/loader.h"
#include "tensorflow_serving/resources/resources.pb.h"
#include "tensorflow_serving/servables/tensorflow/session_bundle_config.pb.h"

namespace tensorflow {
namespace serving {

/// A factory that creates SavedModelBundlesV2 from SavedModel or SessionBundle
/// export paths.
///
/// The emitted sessions only support Run(), and although not enforced it is
/// expected that the client will only make non-mutating Run() calls. (If this
/// restriction, which we've added as a safety measure, is problematic for your
/// use-case please contact the TensorFlow Serving team to discuss disabling
/// it.)
///
/// If the config calls for batching, the emitted sessions automatically batch
/// Run() calls behind the scenes, using a SharedBatchScheduler owned by the
/// factory. The 'config.num_batch_threads' threads are shared across all
/// session instances created by this factory. However, each session has its own
/// dedicated queue of size 'config.max_enqueued_batches'.
///
/// The factory can also estimate the resource (e.g. RAM) requirements of a
/// SavedModelBundleV2 based on the SavedModel (i.e. prior to loading the
/// session).
///
/// This class is thread-safe.
class SavedModelBundleV2Factory {
 public:
  /// Instantiates a SavedModelBundleV2Factory using a config.
  ///
  /// @param config    Config with initialization options.
  /// @param factory   Newly created factory if the returned Status is OK.
  static Status Create(const SessionGroupBundleConfig& config,
                       std::unique_ptr<SavedModelBundleV2Factory>* factory);

  /// Instantiates a bundle from a given export or SavedModel path.
  ///
  /// @param path      Path to the model.
  /// @param bundle    Newly created SavedModelBundleV2 if the returned Status is
  /// OK.
  Status CreateSavedModelBundleV2(const string& path, int model_id,
                                  std::unique_ptr<SavedModelBundleV2>* bundle);

  /// Instantiates a bundle from a given export or SavedModel path and the given
  /// metadata.
  ///
  /// @param metadata  Metadata to be associated with the bundle.
  /// @param path      Path to the model.
  /// @param bundle    Newly created SavedModelBundleV2 if the returned Status is
  /// OK.
  Status CreateSavedModelBundleV2WithMetadata(
      const Loader::Metadata& metadata, const string& path,
      int model_id, std::unique_ptr<SavedModelBundleV2>* bundle);

  /// Estimates the resources a SavedModel bundle will use once loaded, from its
  /// export path.
  ///
  /// @param path      Path to the model.
  /// @param estimate  Output resource usage estimates. Different kinds of
  /// resources (e.g. CPU, RAM, etc.) may get populated.
  Status EstimateResourceRequirement(const string& path,
                                     ResourceAllocation* estimate) const;

  const SessionGroupBundleConfig& config() const { return config_; }

 private:
  using Batcher = SharedBatchScheduler<BatchingSessionTask>;

  SavedModelBundleV2Factory(const SessionGroupBundleConfig& config,
                            std::shared_ptr<Batcher> batch_scheduler);

  Status InternalCreateSavedModelBundleV2(
      const absl::optional<Loader::Metadata>& metadata, const string& path,
      int model_id, std::unique_ptr<SavedModelBundleV2>* bundle);

  const SessionGroupBundleConfig config_;

  // A shared batch scheduler. One queue is used for each session this factory
  // emits. If batching is not configured, this remains null.
  std::shared_ptr<Batcher> batch_scheduler_;

  TF_DISALLOW_COPY_AND_ASSIGN(SavedModelBundleV2Factory);
};

}  // namespace serving
}  // namespace tensorflow

#endif  // TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_FACTORY_H_
