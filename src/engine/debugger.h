#pragma once

#include <windows.h>

namespace memscope {

bool AttachDebugger(DWORD pid);
bool DetachDebugger(DWORD pid);
bool SetHardwareBreakpoint(HANDLE threadHandle, uintptr_t address, int slot);

}  // namespace memscope