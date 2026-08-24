#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>
#include <atomic>
#include <thread>

namespace memscope
{

    struct MemoryRegion
    {
        uintptr_t baseAddress;
        size_t size;
    };

    std::vector<MemoryRegion> GetReadableWritableRegions(HANDLE processHandle);
    std::vector<uintptr_t> ScanRegionForValue(HANDLE processHandle, const MemoryRegion &region, int32_t targetValue);
    std::vector<uintptr_t> RescanAddresses(HANDLE processHandle, const std::vector<uintptr_t> &previousMatches, int32_t targetValue);
    bool WriteValueToAddress(HANDLE processHandle, uintptr_t address, int32_t value);

    class FreezeWorker
    {
    public:
        void Start(HANDLE processHandle, uintptr_t address, int32_t value);
        void Stop();

    private:
        std::atomic<bool> running{false};
        std::thread workerThread;
    };

}