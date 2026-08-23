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

}