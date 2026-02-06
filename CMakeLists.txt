cmake_minimum_required(VERSION 3.14)
project(ClothSimulation CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix for missing C++ headers on some macOS environments
if(APPLE)
    execute_process(COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(SDK_PATH)
        include_directories(SYSTEM ${SDK_PATH}/usr/include/c++/v1)
    endif()
endif()

# Dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# ImGui via FetchContent
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG        v1.90.1
)
FetchContent_MakeAvailable(imgui)

# Sources
set(SOURCES
    Main.cpp
    Cloth.cpp
    # Particle.cpp # Removing Particle class
    # Spring.cpp # Removed Spring.cpp
)

add_executable(ClothSim ${SOURCES})

# Include directories
target_include_directories(ClothSim PRIVATE 
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

# Link libraries
# Link libraries
target_link_libraries(ClothSim PRIVATE
    glfw
    GLEW::GLEW
    glm::glm
    Eigen3::Eigen
    OpenGL::GL
)

# ImGui sources (Core + Backends)
target_sources(ClothSim PRIVATE
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ClothSim PRIVATE ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
