add_library(imgui STATIC
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/imgui_demo.cpp

    # Backends (adjust if you're not using GLFW + OpenGL)
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui/backends
)

target_link_libraries(imgui PUBLIC
    glfw
    glad
)

add_library(imguizmo STATIC
    ${CMAKE_SOURCE_DIR}/dependencies/include/imguizmo/ImGuizmo.cpp
)

target_include_directories(imguizmo PUBLIC
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/include/imgui
    ${CMAKE_SOURCE_DIR}/dependencies/include/imguizmo
)

target_link_libraries(imguizmo PUBLIC
    imgui
)

add_library(dependencies INTERFACE)

target_link_libraries(dependencies INTERFACE
    includes
    glfw
    glad
    imgui
    imguizmo
)

add_library(includes INTERFACE)
target_include_directories(includes INTERFACE
    ${CMAKE_SOURCE_DIR}/dependencies/include
)

add_library(glfw INTERFACE)
target_link_libraries(glfw INTERFACE
    ${CMAKE_SOURCE_DIR}/dependencies/lib/glfw3.lib
    #This is inside windows directly
    opengl32.lib
)

#GLAD needs glad.h, which is in the include directory
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/dependencies/glad.c)
target_link_libraries(glad PRIVATE dependencies)