# MemScope
Debugging tool inspired by [CheatEngine](https://github.com/cheat-engine/cheat-engine)

## How it works 
- Attaches to a target process and enumerates read- and writable memory regions
- Scans memory for a given value and narrows down matches by doing multiple scans
- Freezes a value in memory via a background thread that continuously rewrites it
- Refuses to run if a known anti-cheat process is detected
- Qt GUI

## Status
Work in progress. 
Process attach, scanning, freezing, and a basic GUI are working 
Currently working on automatic pointer chain solution via hardware breakpoints

## Known issues
- Windows Defender or other antivirus software might flag this program 
  That is expected for tools like this and is not a bug
- Reading and Writing memory, even in singleplayer games is often against a game's EULA

## Tech stack
- C++20, WinAPI (ReadProcessMemory, WriteProcessMemory, Toolhelp32Snapshot, debug registers)
- Qt 6 for the GUI
- CMake, built with MSVC

## Building
```
mkdir build
cd build
cmake ..
cmake --build .
```
