#include <windows.h>
#include <tlhelp32.h>

#include <iostream>

#include "anticheat_guard.h"
#include "debugger.h"
#include "feature_catalog.h"
#include "memory_scanner.h"
#include "process_finder.h"

int main() {
  if (memscope::IsAntiCheatProcessRunning()) {
    std::wcout << L"Anti-cheat process detected. MemScope refuses to run."
               << std::endl;
    return 1;
  }

  int32_t testValue = 123456789;

  DWORD ownPid = GetCurrentProcessId();
  HANDLE processHandle = memscope::OpenProcessByPid(ownPid);

  if (processHandle == NULL) {
    std::wcout << L"Failed to open own process handle." << std::endl;
    return 1;
  }

  std::wcout << L"Attached to own process, PID: " << ownPid << std::endl;
  std::wcout << L"Known test value is at address: " << &testValue << std::endl;

  auto regions = memscope::GetReadableWritableRegions(processHandle);

  std::vector<uintptr_t> allMatches;
  for (const auto& region : regions) {
    auto matches =
        memscope::ScanRegionForValue(processHandle, region, testValue);
    allMatches.insert(allMatches.end(), matches.begin(), matches.end());
  }

  std::wcout << L"First scan found " << allMatches.size()
             << L" matches for value " << testValue << L":" << std::endl;
  for (const auto& address : allMatches) {
    std::wcout << L"  0x" << std::hex << address << std::dec << std::endl;
  }

  testValue = 999999999;
  std::wcout << L"Value changed to " << testValue << L", rescanning..."
             << std::endl;

  auto narrowedMatches =
      memscope::RescanAddresses(processHandle, allMatches, testValue);

  std::wcout << L"Rescan narrowed down to " << narrowedMatches.size()
             << L" match(es):" << std::endl;
  for (const auto& address : narrowedMatches) {
    std::wcout << L"  0x" << std::hex << address << std::dec << std::endl;
  }

  if (!narrowedMatches.empty()) {
    uintptr_t targetAddress = narrowedMatches[0];
    int32_t newValue = 42;

    bool writeSuccess =
        memscope::WriteValueToAddress(processHandle, targetAddress, newValue);
    std::wcout << L"Write to 0x" << std::hex << targetAddress << std::dec
               << (writeSuccess ? L" succeeded." : L" failed.") << std::endl;

    std::wcout << L"testValue in memory is now: " << testValue << std::endl;

    memscope::FreezeWorker freezeWorker;
    freezeWorker.Start(processHandle, targetAddress, 100);

    std::wcout << L"Freezing value at 0x" << std::hex << targetAddress
               << std::dec << L" to 100. Press Enter to stop." << std::endl;

    std::cin.get();

    freezeWorker.Stop();
    std::wcout << L"testValue in memory is now: " << testValue << std::endl;

    memscope::FeatureCatalog catalog;

    memscope::Feature freezeFeature;
    freezeFeature.name = "Test Freeze Feature";
    freezeFeature.category = memscope::FeatureCategory::ValueFreeze;
    freezeFeature.isBuiltIn = false;
    freezeFeature.targets.push_back({targetAddress, 100});

    catalog.AddFeature(freezeFeature);

    std::wcout << L"Catalog has " << catalog.GetFeatures().size()
               << L" feature(s)." << std::endl;

    for (const auto& feature : catalog.GetFeatures()) {
      std::wcout << L"  Feature: " << feature.name.c_str() << L", targets: "
                 << feature.targets.size() << std::endl;
    }

    auto pid = memscope::FindProcessIdByName(L"memscope_test_target.exe");

    if (pid.has_value()) {
      if (memscope::AttachDebugger(pid.value())) {
        std::wcout << L"Debugger attached!" << std::endl;

        HANDLE handle = memscope::OpenProcessByPid(pid.value());

        std::vector<HANDLE> allThreads;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        THREADENTRY32 threadEntry{};
        threadEntry.dwSize = sizeof(THREADENTRY32);

        if (Thread32First(snapshot, &threadEntry)) {
          do {
            if (threadEntry.th32OwnerProcessID == pid.value()) {
              HANDLE t = OpenThread(THREAD_ALL_ACCESS, FALSE,
                                    threadEntry.th32ThreadID);
              if (t != NULL) {
                allThreads.push_back(t);
              }
            }
          } while (Thread32Next(snapshot, &threadEntry));
        }
        CloseHandle(snapshot);

        if (!allThreads.empty() && handle != NULL) {
          uintptr_t testAddress = 0x15FDC4;

          bool anySuccess = false;
          for (auto t : allThreads) {
            if (memscope::SetHardwareBreakpoint(t, testAddress, 0)) {
              anySuccess = true;
            }
            CloseHandle(t);
          }

          std::wcout << (anySuccess ? L"Breakpoint set successfully."
                                    : L"Failed to set breakpoint.")
                     << std::endl;

          if (anySuccess) {
            std::wcout << L"Waiting for breakpoint hit.." << std::endl;

            uintptr_t hitAddress = 0;
            bool hit =
                memscope::WaitForBreakpointHit(pid.value(), 10000, hitAddress);

            if (hit) {
              std::wcout << L"Breakpoint hit pointer: 0x" << std::hex
                         << hitAddress << std::dec << std::endl;
            } else {
              std::wcout << L"No breakpoint hit." << std::endl;
            }
          }
        }

        if (handle != NULL) {
          CloseHandle(handle);
        }

        memscope::DetachDebugger(pid.value());
        std::wcout << L"Debugger detached." << std::endl;
      } else {
        std::wcout << L"Failed to attach debugger." << std::endl;
      }
    } else {
      std::wcout << L"Test target not found!" << std::endl;
    }
  }

  CloseHandle(processHandle);

  return 0;
}