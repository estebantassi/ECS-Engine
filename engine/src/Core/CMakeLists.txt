add_library(core core.cpp types.cpp "input.cpp")

target_link_libraries(core PUBLIC engine)