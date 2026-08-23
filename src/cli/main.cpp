#include "process_finder.h"
#include "anticheat_guard.h"
#include "memory_scanner.h"

#include <iostream>
#include <windows.h>

int main()
{
    if (memscope::IsAntiCheatProcessRunning())
    {
        std::wcout << L"Anti-cheat process detected. MemScope refuses to run." << std::endl;
        return 1;
    }

    int32_t testValue = 123456789;

    DWORD ownPid = GetCurrentProcessId();
    HANDLE processHandle = memscope::OpenProcessByPid(ownPid);

    if (processHandle == NULL)
    {
        std::wcout << L"Failed to open own process handle." << std::endl;
        return 1;
    }

    std::wcout << L"Attached to own process, PID: " << ownPid << std::endl;
    std::wcout << L"Known test value is at address: " << &testValue << std::endl;

    auto regions = memscope::GetReadableWritableRegions(processHandle);
    std::wcout << L"Found " << regions.size() << L" readable/writable regions." << std::endl;

    std::vector<uintptr_t> allMatches;
    for (const auto &region : regions)
    {
        auto matches = memscope::ScanRegionForValue(processHandle, region, testValue);
        allMatches.insert(allMatches.end(), matches.begin(), matches.end());
    }

    std::wcout << L"Scan found " << allMatches.size() << L" matches for value " << testValue << L":" << std::endl;
    for (const auto &address : allMatches)
    {
        std::wcout << L"  0x" << std::hex << address << std::dec << std::endl;
    }

    CloseHandle(processHandle);

    return 0;
}