add_library(renderer renderer.cpp lightsmanager.cpp)

target_link_libraries(renderer PUBLIC engine)