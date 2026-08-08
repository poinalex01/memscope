add_executable(memscope_cli
    main.cpp
)

target_link_libraries(memscope_cli PRIVATE memscope_engine)
