# CMakeLists.txt in UserProject
cmake_minimum_required(VERSION 3.28)
project(BezierEditor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHICS_LAB_INSTALL_PATH "" CACHE STRING "install path of graphics lab")
set(PYTHON_PATH "" CACHE STRING "install path of graphics lab")

set(${Python3_EXECUTABLE} ${PYTHON_PATH}/python.exe)
set(${Python3_INCLUDE_DIRS} ${PYTHON_PATH}/include)

list(APPEND CMAKE_PREFIX_PATH ${GRAPHICS_LAB_INSTALL_PATH})
list(APPEND CMAKE_PREFIX_PATH ${GRAPHICS_LAB_INSTALL_PATH}/lib/cmake/igl)
INCLUDE_DIRECTORIES(${GRAPHICS_LAB_INSTALL_PATH}/include)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

LINK_DIRECTORIES(${PYTHON_PATH}/libs)


message(STATUS "Graphics Lab Install Path: ${GRAPHICS_LAB_INSTALL_PATH}")

# Path to the graphics engine's root directory
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(spirv_cross_core REQUIRED)
find_package(spirv_cross_glsl REQUIRED)
find_package(spirv_cross_cpp REQUIRED)
find_package(autodiff REQUIRED)
find_package(eigen3 REQUIRED)
find_package(libigl REQUIRED)
find_package(OpenMP REQUIRED)
find_package(GraphicsLab REQUIRED)
find_package(OpenCV)

find_package(OpenMP REQUIRED)

include(fetch-pybind)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# set vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "VK_SDK_PATH not defined!")
endif()

set(VK_SDK_PATH $ENV{VULKAN_SDK})
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

if (UNIX)
    message(STATUS "Set Linux Vulkan SDK Path")
    set(VK_SDK_LIB ${VK_SDK_PATH}/lib/libvulkan.so.1)
endif ()

file(GLOB Boost_INCLUDE_DIRS LIST_DIRECTORIES true ${GRAPHICS_LAB_INSTALL_PATH}/boost/include)

# Create the user project executable
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

add_definitions(-DMODEL_PATH="${CMAKE_SOURCE_DIR}/external/models")

target_include_directories(${PROJECT_NAME} PUBLIC ${GRAPHICS_LAB_INSTALL_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${VK_SDK_INCLUDE})
target_include_directories(${PROJECT_NAME} PUBLIC  ${GRAPHICS_LAB_INSTALL_PATH}/include/boost/include)

message(STATUS ${Python3_LIBRARIES})

# Link the engine's libraries (replace YourEngineLib with actual library names)
target_link_libraries(${PROJECT_NAME}
        PUBLIC GraphicsLab::LAB_vkl ${VK_SDK_LIB} igl::igl_core OpenMP::OpenMP_CXX pybind11::embed
)

#target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::module pybind11::windows_extras)

# Path to the JSON file
set(OUTPUT_JSON "${CMAKE_CURRENT_SOURCE_DIR}/GraphicsLabProject.json")

# Define the Python script to update the JSON
set(UPDATE_JSON_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/manager.py")

# Add post-build step to update JSON with DLL path
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=$ENV{PYTHONPATH} ${Python3_EXECUTABLE} ${UPDATE_JSON_SCRIPT}
        --project-dir "${CMAKE_CURRENT_SOURCE_DIR}"
        --output-dll "$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        --build-type "$<CONFIG>"  # CONFIG will be Debug, Release, etc.
        COMMENT "Updating ${OUTPUT_JSON} with DLL path and build type"
)
