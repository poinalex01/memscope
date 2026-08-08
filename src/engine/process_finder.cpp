#include "process_finder.h"

#include <tlhelp32.h>

namespace memscope
{

    std::optional<DWORD> FindProcessIdByName(const std::wstring &processName)
    {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
        {
            return std::nullopt;
        }

        PROCESSENTRY32W entry{};
        entry.dwSize = sizeof(PROCESSENTRY32W);

        std::optional<DWORD> result;

        if (Process32FirstW(snapshot, &entry))
        {
            do
            {
                if (processName == entry.szExeFile)
                {
                    result = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return result;
    }

}