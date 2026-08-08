#include "process_finder.h"

#include <iostream>

int main()
{
    std::wstring targetName = L"Notepad.exe";

    auto pid = memscope::FindProcessIdByName(targetName);

    if (pid.has_value())
    {
        std::wcout << L"Found process '" << targetName << L"' with PID: " << pid.value() << std::endl;
    }
    else
    {
        std::wcout << L"Process '" << targetName << L"' not found." << std::endl;
    }

    return 0;
}