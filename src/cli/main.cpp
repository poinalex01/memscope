#include "process_finder.h"
#include "anticheat_guard.h"

#include <iostream>
#include <windows.h>

int main()
{
    if (memscope::IsAntiCheatProcessRunning())
    {
        std::wcout << L"Anti-cheat process detected. MemScope refuses to run." << std::endl;
        return 1;
    }

    std::wstring targetName = L"Notepad.exe";

    auto pid = memscope::FindProcessIdByName(targetName);

    if (!pid.has_value())
    {
        std::wcout << L"Process '" << targetName << L"' not found." << std::endl;
        return 1;
    }

    std::wcout << L"Found process '" << targetName << L"' with PID: " << pid.value() << std::endl;

    HANDLE processHandle = memscope::OpenProcessByPid(pid.value());

    if (processHandle == NULL)
    {
        std::wcout << L"Failed to open process handle." << std::endl;
        return 1;
    }

    std::wcout << L"Process handle opened successfully." << std::endl;

    CloseHandle(processHandle);

    return 0;
}