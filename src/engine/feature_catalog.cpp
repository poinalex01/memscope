#include "feature_catalog.h"

namespace memscope {

void FeatureCatalog::AddFeature(const Feature& feature) {
  features.push_back(feature);
}

const std::vector<Feature>& FeatureCatalog::GetFeatures() const {
  return features;
}

}  // namespace memscope