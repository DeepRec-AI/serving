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

#ifndef TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_SOURCE_ADAPTER_H_
#define TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_SOURCE_ADAPTER_H_

#include "tensorflow/cc/saved_model/loader.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow_serving/core/loader.h"
#include "tensorflow_serving/core/simple_loader.h"
#include "tensorflow_serving/core/source_adapter.h"
#include "tensorflow_serving/core/storage_path.h"
#include "tensorflow_serving/servables/tensorflow/saved_model_bundle_v2_factory.h"
#include "tensorflow_serving/servables/tensorflow/saved_model_bundle_source_adapter.pb.h"
#include "tensorflow_serving/servables/tensorflow/session_bundle_source_adapter.pb.h"

namespace tensorflow {
namespace serving {

// A SourceAdapter that creates SavedModelBundleV2 Loaders from SavedModel paths.
// It keeps a SavedModelBundleV2Factory as its state, which may house a batch
// scheduler that is shared across all of the SavedModel bundles it emits.
class SavedModelBundleV2SourceAdapter final
    : public UnarySourceAdapter<StoragePath, std::unique_ptr<Loader>> {
 public:
  // TODO(b/32248363): Switch to SavedModelBundleV2SourceAdapterConfig after we
  // switch Model Server to Saved Model and populate the "real" fields of
  // SavedModelBundleV2SourceAdapterConfig.
  static Status Create(const SessionGroupBundleSourceAdapterConfig& config,
                       std::unique_ptr<SavedModelBundleV2SourceAdapter>* adapter);

  ~SavedModelBundleV2SourceAdapter() override;

  // Returns a function to create a SavedModel bundle source adapter.
  static std::function<Status(
      std::unique_ptr<SourceAdapter<StoragePath, std::unique_ptr<Loader>>>*)>
  GetCreator(const SessionGroupBundleSourceAdapterConfig& config);

 private:
  friend class SavedModelBundleV2SourceAdapterCreator;

  explicit SavedModelBundleV2SourceAdapter(
      std::unique_ptr<SavedModelBundleV2Factory> bundle_factory);

  SimpleLoader<SavedModelBundleV2>::CreatorVariant GetServableCreator(
      std::shared_ptr<SavedModelBundleV2Factory> bundle_factory,
      const StoragePath& path, int model_id) const;

  Status Convert(const StoragePath& path,
                 std::unique_ptr<Loader>* loader) override;

  Status Convert(const StoragePath& path, int model_id,
                 std::unique_ptr<Loader>* loader) override;

  // We use a shared ptr to share ownership with Loaders we emit, in case they
  // outlive this object.
  std::shared_ptr<SavedModelBundleV2Factory> bundle_factory_;

  TF_DISALLOW_COPY_AND_ASSIGN(SavedModelBundleV2SourceAdapter);
};

}  // namespace serving
}  // namespace tensorflow

#endif  // TENSORFLOW_SERVING_SERVABLES_TENSORFLOW_SAVED_MODEL_BUNDLE_V2_SOURCE_ADAPTER_H_
