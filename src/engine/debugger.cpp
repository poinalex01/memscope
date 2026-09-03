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

}  // namespace memscope