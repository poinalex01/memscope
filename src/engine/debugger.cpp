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

  // Set condition to read or write (bits 16-17 for slot 0) and length to 4
  // bytes (bits 18-19).
  int conditionOffset = 16 + (slot * 4);
  context.Dr7 &= ~(0xFull << conditionOffset);
  context.Dr7 |= (0x3ull << conditionOffset);  // 0b11 = break on read or write
  context.Dr7 |= (0x3ull << (conditionOffset + 2));  // 0b11 = 4 byte length

  return SetThreadContext(threadHandle, &context) != 0;
}

bool WaitForBreakpointHit(DWORD pid, DWORD timeoutMs,
                          uintptr_t& hitInstructionPointer) {
  DEBUG_EVENT debugEvent{};
  ULONGLONG startTime = GetTickCount64();

  while (true) {
    ULONGLONG elapsed = GetTickCount64() - startTime;
    if (elapsed >= timeoutMs) {
      return false;
    }

    DWORD remaining = static_cast<DWORD>(timeoutMs - elapsed);

    if (!WaitForDebugEvent(&debugEvent, remaining)) {
      return false;
    }

    if (debugEvent.dwProcessId != pid) {
      ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId,
                         DBG_CONTINUE);
      continue;
    }

    if (debugEvent.dwDebugEventCode == EXCEPTION_DEBUG_EVENT &&
        debugEvent.u.Exception.ExceptionRecord.ExceptionCode ==
            EXCEPTION_SINGLE_STEP) {
      HANDLE threadHandle =
          OpenThread(THREAD_ALL_ACCESS, FALSE, debugEvent.dwThreadId);

      CONTEXT context{};
      context.ContextFlags = CONTEXT_CONTROL;
      GetThreadContext(threadHandle, &context);

      hitInstructionPointer = context.Rip;

      CloseHandle(threadHandle);
      ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId,
                         DBG_CONTINUE);
      return true;
    }

    ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId,
                       DBG_CONTINUE);
  }
}

}  // namespace memscope