cmake_minimum_required(VERSION 3.26)
project(VRT)

#Folders used by the project overall
add_subdirectory(engine)
add_subdirectory(dependencies)

#Entry point
add_executable(VRT
    engine/src/main.cpp
)

target_link_libraries(VRT PRIVATE
    engine
)