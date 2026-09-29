cmake_minimum_required(VERSION 3.16)
project(Geometry)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Geometry
    src/main.cpp
    src/math.cpp

    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp

    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(Geometry PRIVATE
    imgui
    imgui/backends
)

target_link_libraries(Geometry
    glfw
    OpenGL::GL
)