#pragma once

#include <windows.h>
#include <vector>
#include <cstdint>

namespace memscope
{

    struct MemoryRegion
    {
        uintptr_t baseAddress;
        size_t size;
    };

    std::vector<MemoryRegion> GetReadableWritableRegions(HANDLE processHandle);

}