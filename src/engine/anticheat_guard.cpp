#include "anticheat_guard.h"

#include <tlhelp32.h>

#include <algorithm>
#include <cwctype>

namespace memscope {

namespace {

const std::vector<std::wstring>& GetKnownAntiCheatProcessNames() {
  static const std::vector<std::wstring> knownProcesses = {
      L"EasyAntiCheat.exe", L"BEService.exe", L"BattlEye.exe", L"vgc.exe",
      L"vgk.exe",           L"FACEIT.exe",    L"ACE-Tray.exe"};
  return knownProcesses;
}

bool EqualsCaseInsensitive(const std::wstring& a, const std::wstring& b) {
  if (a.size() != b.size()) {
    return false;
  }
  return std::equal(a.begin(), a.end(), b.begin(), [](wchar_t c1, wchar_t c2) {
    return std::towlower(c1) == std::towlower(c2);
  });
}

}  // namespace

bool IsAntiCheatProcessRunning() {
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return false;
  }

  PROCESSENTRY32W entry{};
  entry.dwSize = sizeof(PROCESSENTRY32W);

  bool found = false;

  if (Process32FirstW(snapshot, &entry)) {
    do {
      for (const auto& knownName : GetKnownAntiCheatProcessNames()) {
        if (EqualsCaseInsensitive(entry.szExeFile, knownName)) {
          found = true;
          break;
        }
      }
      if (found) {
        break;
      }
    } while (Process32NextW(snapshot, &entry));
  }

  CloseHandle(snapshot);
  return found;
}

}  // namespace memscope