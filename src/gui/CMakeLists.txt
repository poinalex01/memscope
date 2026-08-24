find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(memscope_gui
    main.cpp
)

target_link_libraries(memscope_gui PRIVATE Qt6::Widgets memscope_engine)