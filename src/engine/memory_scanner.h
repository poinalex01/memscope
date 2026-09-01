#pragma once

#include <windows.h>

#include <atomic>
#include <cstdint>
#include <thread>
#include <variant>
#include <vector>

namespace memscope {

struct MemoryRegion {
  uintptr_t baseAddress;
  size_t size;
};

using ScanValue = std::variant<int32_t, int64_t, float, double>;
std::vector<MemoryRegion> GetReadableWritableRegions(HANDLE processHandle);
std::vector<uintptr_t> ScanRegionForValue(HANDLE processHandle,
                                          const MemoryRegion& region,
                                          const ScanValue& targetValue);
std::vector<uintptr_t> RescanAddresses(
    HANDLE processHandle, const std::vector<uintptr_t>& previousMatches,
    const ScanValue& targetValue);
bool WriteValueToAddress(HANDLE processHandle, uintptr_t address,
                         const ScanValue& value);

class FreezeWorker {
 public:
  void Start(HANDLE processHandle, uintptr_t address, const ScanValue& value);
  void Stop();

 private:
  std::atomic<bool> running{false};
  std::thread workerThread;
};

}  // namespace memscope