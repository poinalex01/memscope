#include "memory_scanner.h"

#include <cmath>
#include <cstring>

namespace memscope {

std::vector<MemoryRegion> GetReadableWritableRegions(HANDLE processHandle) {
  std::vector<MemoryRegion> regions;

  MEMORY_BASIC_INFORMATION mbi{};
  uintptr_t address = 0;

  while (VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(address), &mbi,
                        sizeof(mbi)) == sizeof(mbi)) {
    bool isCommitted = mbi.State == MEM_COMMIT;
    bool isReadWrite =
        mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE;
    bool isPrivate = mbi.Type == MEM_PRIVATE;

    if (isCommitted && isReadWrite && isPrivate) {
      MemoryRegion region;
      region.baseAddress = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
      region.size = mbi.RegionSize;
      regions.push_back(region);
    }

    address = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
  }

  return regions;
}

namespace {

bool ValuesMatch(const ScanValue& a, const ScanValue& b) {
  if (std::holds_alternative<float>(a)) {
    return std::abs(std::get<float>(a) - std::get<float>(b)) < 0.01f;
  }
  if (std::holds_alternative<double>(a)) {
    return std::abs(std::get<double>(a) - std::get<double>(b)) < 0.01;
  }
  return a == b;
}

size_t SizeOfScanValue(const ScanValue& value) {
  return std::visit([](auto&& v) { return sizeof(v); }, value);
}

}  // namespace

std::vector<uintptr_t> ScanRegionForValue(HANDLE processHandle,
                                          const MemoryRegion& region,
                                          const ScanValue& targetValue) {
  std::vector<uintptr_t> matches;

  size_t valueSize = SizeOfScanValue(targetValue);

  if (region.size < valueSize) {
    return matches;
  }

  std::vector<uint8_t> buffer(region.size);
  SIZE_T bytesRead = 0;

  bool success = ReadProcessMemory(
      processHandle, reinterpret_cast<LPCVOID>(region.baseAddress),
      buffer.data(), region.size, &bytesRead);

  if (!success) {
    return matches;
  }

  for (size_t offset = 0; offset + valueSize <= bytesRead;
       offset += valueSize) {
    bool matched = std::visit(
        [&](auto&& typedTarget) {
          using T = std::decay_t<decltype(typedTarget)>;
          T candidate;
          std::memcpy(&candidate, buffer.data() + offset, sizeof(T));
          return ValuesMatch(ScanValue(candidate), ScanValue(typedTarget));
        },
        targetValue);

    if (matched) {
      matches.push_back(region.baseAddress + offset);
    }
  }

  return matches;
}

std::vector<uintptr_t> RescanAddresses(
    HANDLE processHandle, const std::vector<uintptr_t>& previousMatches,
    int32_t targetValue) {
  std::vector<uintptr_t> stillMatching;

  for (const auto& address : previousMatches) {
    int32_t currentValue = 0;
    SIZE_T bytesRead = 0;

    bool success =
        ReadProcessMemory(processHandle, reinterpret_cast<LPCVOID>(address),
                          &currentValue, sizeof(int32_t), &bytesRead);

    if (success && bytesRead == sizeof(int32_t) &&
        currentValue == targetValue) {
      stillMatching.push_back(address);
    }
  }

  return stillMatching;
}

bool WriteValueToAddress(HANDLE processHandle, uintptr_t address,
                         int32_t value) {
  SIZE_T bytesWritten = 0;

  bool success =
      WriteProcessMemory(processHandle, reinterpret_cast<LPVOID>(address),
                         &value, sizeof(int32_t), &bytesWritten);

  return success && bytesWritten == sizeof(int32_t);
}

void FreezeWorker::Start(HANDLE processHandle, uintptr_t address,
                         int32_t value) {
  running = true;

  workerThread = std::thread([this, processHandle, address, value]() {
    while (running) {
      WriteValueToAddress(processHandle, address, value);
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  });
}

void FreezeWorker::Stop() {
  running = false;

  if (workerThread.joinable()) {
    workerThread.join();
  }
}

}  // namespace memscope