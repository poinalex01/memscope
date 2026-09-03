#pragma once

#include <windows.h>

namespace memscope {

bool AttachDebugger(DWORD pid);
bool DetachDebugger(DWORD pid);

}  // namespace memscope