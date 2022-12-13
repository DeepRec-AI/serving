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

#include "tensorflow_serving/servables/tensorflow/saved_model_bundle_v2_source_adapter.h"

#include <memory>
#include <string>

#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow_serving/resources/resource_util.h"
#include "tensorflow_serving/resources/resource_values.h"
#include "tensorflow_serving/resources/resources.pb.h"
#include "tensorflow_serving/servables/tensorflow/bundle_factory_util.h"
#include "tensorflow_serving/servables/tensorflow/saved_model_bundle_v2_factory.h"
#include "tensorflow_serving/servables/tensorflow/saved_model_warmup.h"
#include "tensorflow_serving/util/optional.h"

namespace tensorflow {
namespace serving {

Status SavedModelBundleV2SourceAdapter::Create(
    const SessionGroupBundleSourceAdapterConfig& config,
    std::unique_ptr<SavedModelBundleV2SourceAdapter>* adapter) {
  std::unique_ptr<SavedModelBundleV2Factory> bundle_factory;
  TF_RETURN_IF_ERROR(
      SavedModelBundleV2Factory::Create(config.config(), &bundle_factory));
  adapter->reset(new SavedModelBundleV2SourceAdapter(std::move(bundle_factory)));
  return Status::OK();
}

SavedModelBundleV2SourceAdapter::~SavedModelBundleV2SourceAdapter() { Detach(); }

SavedModelBundleV2SourceAdapter::SavedModelBundleV2SourceAdapter(
    std::unique_ptr<SavedModelBundleV2Factory> bundle_factory)
    : bundle_factory_(std::move(bundle_factory)) {}

SimpleLoader<SavedModelBundleV2>::CreatorVariant
SavedModelBundleV2SourceAdapter::GetServableCreator(
    std::shared_ptr<SavedModelBundleV2Factory> bundle_factory,
    const StoragePath& path, int model_id) const {
  if (bundle_factory->config().enable_session_metadata()) {
    return [bundle_factory, path, model_id](const Loader::Metadata& metadata,
                                  std::unique_ptr<SavedModelBundleV2>* bundle) {
      TF_RETURN_IF_ERROR(bundle_factory->CreateSavedModelBundleV2WithMetadata(
          metadata, path, model_id, bundle));
      if (bundle_factory->config().enable_model_warmup()) {
        return RunSavedModelWarmup(
            bundle_factory->config().model_warmup_options(),
            GetRunOptions(bundle_factory->config()), path, bundle->get());
      }
      return Status::OK();
    };
  }
  return [bundle_factory, path, model_id](std::unique_ptr<SavedModelBundleV2>* bundle) {
    TF_RETURN_IF_ERROR(bundle_factory->CreateSavedModelBundleV2(path, model_id, bundle));
    if (bundle_factory->config().enable_model_warmup()) {
      return RunSavedModelWarmup(
          bundle_factory->config().model_warmup_options(),
          GetRunOptions(bundle_factory->config()), path, bundle->get());
    }
    return Status::OK();
  };
}

Status SavedModelBundleV2SourceAdapter::Convert(const StoragePath& path,
                                                std::unique_ptr<Loader>* loader) {
  return Convert(path, 0, loader);
}

Status SavedModelBundleV2SourceAdapter::Convert(const StoragePath& path, int model_id,
                                                std::unique_ptr<Loader>* loader) {
  std::shared_ptr<SavedModelBundleV2Factory> bundle_factory = bundle_factory_;
  auto servable_creator = GetServableCreator(bundle_factory, path, model_id);
  auto resource_estimator = [bundle_factory,
                             path](ResourceAllocation* estimate) {
    TF_RETURN_IF_ERROR(
        bundle_factory->EstimateResourceRequirement(path, estimate));

    // Add experimental_transient_ram_bytes_during_load.
    // TODO(b/38376838): Remove once resource estimates are moved inside
    // SavedModel.
    ResourceUtil::Options resource_util_options;
    resource_util_options.devices = {{device_types::kMain, 1}};
    std::unique_ptr<ResourceUtil> resource_util =
        std::unique_ptr<ResourceUtil>(new ResourceUtil(resource_util_options));
    const Resource ram_resource = resource_util->CreateBoundResource(
        device_types::kMain, resource_kinds::kRamBytes);
    resource_util->SetQuantity(
        ram_resource,
        resource_util->GetQuantity(ram_resource, *estimate) +
            bundle_factory->config()
                .experimental_transient_ram_bytes_during_load(),
        estimate);

    return Status::OK();
  };
  auto post_load_resource_estimator = [bundle_factory,
                                       path](ResourceAllocation* estimate) {
    return bundle_factory->EstimateResourceRequirement(path, estimate);
  };
  loader->reset(new SimpleLoader<SavedModelBundleV2>(
      servable_creator, resource_estimator, {post_load_resource_estimator}));
  return Status::OK();
}

std::function<Status(
    std::unique_ptr<SourceAdapter<StoragePath, std::unique_ptr<Loader>>>*)>
SavedModelBundleV2SourceAdapter::GetCreator(
    const SessionGroupBundleSourceAdapterConfig& config) {
  return [config](std::unique_ptr<tensorflow::serving::SourceAdapter<
                      StoragePath, std::unique_ptr<Loader>>>* source) {
    std::unique_ptr<SavedModelBundleV2SourceAdapter> typed_source;
    TF_RETURN_IF_ERROR(
        SavedModelBundleV2SourceAdapter::Create(config, &typed_source));
    *source = std::move(typed_source);
    return Status::OK();
  };
}

// Register the source adapter.
class SavedModelBundleV2SourceAdapterCreator {
 public:
  static Status Create(
      const SavedModelBundleV2SourceAdapterConfig& config,
      std::unique_ptr<SourceAdapter<StoragePath, std::unique_ptr<Loader>>>*
          adapter) {
    std::unique_ptr<SavedModelBundleV2Factory> bundle_factory;
    TF_RETURN_IF_ERROR(SavedModelBundleV2Factory::Create(config.legacy_config(),
                                                         &bundle_factory));
    adapter->reset(
        new SavedModelBundleV2SourceAdapter(std::move(bundle_factory)));
    return Status::OK();
  }
};
REGISTER_STORAGE_PATH_SOURCE_ADAPTER(SavedModelBundleV2SourceAdapterCreator,
                                     SavedModelBundleV2SourceAdapterConfig);
}  // namespace serving
}  // namespace tensorflow
