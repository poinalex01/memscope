#include "memory_scanner.h"

namespace memscope
{

    std::vector<MemoryRegion> GetReadableWritableRegions(HANDLE processHandle)
    {
        std::vector<MemoryRegion> regions;

        MEMORY_BASIC_INFORMATION mbi{};
        uintptr_t address = 0;

        while (VirtualQueryEx(processHandle, reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) == sizeof(mbi))
        {
            bool isCommitted = mbi.State == MEM_COMMIT;
            bool isReadWrite = mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE;
            bool isPrivate = mbi.Type == MEM_PRIVATE;

            if (isCommitted && isReadWrite && isPrivate)
            {
                MemoryRegion region;
                region.baseAddress = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
                region.size = mbi.RegionSize;
                regions.push_back(region);
            }

            address = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
        }

        return regions;
    }

    std::vector<uintptr_t> ScanRegionForValue(HANDLE processHandle, const MemoryRegion &region, int32_t targetValue)
    {
        std::vector<uintptr_t> matches;

        if (region.size < sizeof(int32_t))
        {
            return matches;
        }

        std::vector<uint8_t> buffer(region.size);
        SIZE_T bytesRead = 0;

        bool success = ReadProcessMemory(
            processHandle,
            reinterpret_cast<LPCVOID>(region.baseAddress),
            buffer.data(),
            region.size,
            &bytesRead);

        if (!success)
        {
            return matches;
        }

        for (size_t offset = 0; offset + sizeof(int32_t) <= bytesRead; offset += sizeof(int32_t))
        {
            int32_t value;
            std::memcpy(&value, buffer.data() + offset, sizeof(int32_t));

            if (value == targetValue)
            {
                matches.push_back(region.baseAddress + offset);
            }
        }

        return matches;
    }

}