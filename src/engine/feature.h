#pragma once

#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace memscope {

enum class FeatureCategory { ValueFreeze, CodePatch };

struct FeatureTarget {
  uintptr_t address;
  int32_t targetValue;
};

struct Feature {
  std::string name;
  FeatureCategory category;
  bool isBuiltIn;
  std::vector<FeatureTarget> targets;
};

}  // namespace memscope