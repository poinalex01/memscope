#pragma once

#include <vector>

#include "feature.h"

namespace memscope {

class FeatureCatalog {
 public:
  void AddFeature(const Feature& feature);
  const std::vector<Feature>& GetFeatures() const;

 private:
  std::vector<Feature> features;
};

}  // namespace memscope