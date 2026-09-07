#include "debugger.h"

namespace memscope {

bool AttachDebugger(DWORD pid) {
  if (!DebugActiveProcess(pid)) {
    return false;
  }

  DebugSetProcessKillOnExit(FALSE);
  return true;
}

bool DetachDebugger(DWORD pid) { return DebugActiveProcessStop(pid) != 0; }

bool SetHardwareBreakpoint(HANDLE threadHandle, uintptr_t address, int slot) {
  CONTEXT context{};
  context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

  if (!GetThreadContext(threadHandle, &context)) {
    return false;
  }

  switch (slot) {
    case 0:
      context.Dr0 = address;
      break;
    case 1:
      context.Dr1 = address;
      break;
    case 2:
      context.Dr2 = address;
      break;
    case 3:
      context.Dr3 = address;
      break;
    default:
      return false;
  }

  context.Dr7 |= (1ull << (slot * 2));

  return SetThreadContext(threadHandle, &context) != 0;
}

}  // namespace memscope