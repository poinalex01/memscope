add_library(memscope_engine STATIC
    process_finder.cpp
    anticheat_guard.cpp
)

target_include_directories(memscope_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
