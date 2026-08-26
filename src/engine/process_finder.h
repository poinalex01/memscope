#pragma once

#include <windows.h>

#include <optional>
#include <string>

namespace memscope {

std::optional<DWORD> FindProcessIdByName(const std::wstring& processName);
HANDLE OpenProcessByPid(DWORD pid);

}  // namespace memscope