#Create engine and use src as its source directory
add_library(engine src src/dummy.cpp  "include/Scene/updatetracker.h" "include/Core/event.h"  "include/Prefab/prefabregistry.h"   "include/Renderer/lighttraits.h" "include/Renderer/lightbuffer.h")

#Add dependencies
add_subdirectory(src/renderer)
add_subdirectory(src/shaders)
add_subdirectory(src/player)
add_subdirectory(src/core)
add_subdirectory(src/ui)
add_subdirectory(src/scene)
add_subdirectory(src/entitymanager)
add_subdirectory(src/prefab)


target_include_directories(engine
    #Every file in the include directory should be available to anyone using the engine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine PUBLIC dependencies PRIVATE renderer shaders player core ui scene entitymanager prefab)

target_compile_definitions(engine PUBLIC
    SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/"
)

target_compile_features(engine PUBLIC cxx_std_23)
set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)