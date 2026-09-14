#pragma once

#include <windows.h>

namespace memscope {

bool AttachDebugger(DWORD pid);
bool DetachDebugger(DWORD pid);
bool SetHardwareBreakpoint(HANDLE threadHandle, uintptr_t address, int slot);
bool WaitForBreakpointHit(DWORD pid, DWORD timeoutMs,
                          uintptr_t& hitInstructionPointer);

}  // namespace memscope