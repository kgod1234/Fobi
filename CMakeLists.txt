# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Add the executable
add_executable(MyProject src/main.cpp src/Mylibs/SimplifyDynamixel.cpp)

# Specify paths to Dynamixel SDK relative to the current folder
set(MY_LIBRARY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/DynamixelSDK-master/c++/include/dynamixel_sdk")
set(MY_LIBRARY_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/DynamixelSDK-master/c++/build/win64/output")
set(MY_LIBRARY_DLL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/DynamixelSDK-master/c++/build/win64/output")

# Specify paths to custom libraries (Mylibs)
set(MY_CUSTOM_LIB_DIR "${CMAKE_SOURCE_DIR}/src/Mylibs")  # Path to your custom Mylibs

# Include directories
target_include_directories(MyProject PRIVATE
    "${MY_LIBRARY_INCLUDE_DIR}"  # External Dynamixel SDK headers
    "${MY_CUSTOM_LIB_DIR}"       # Custom Mylibs headers
)

# Link the Dynamixel SDK import library
target_link_libraries(MyProject PRIVATE "${MY_LIBRARY_LIB_DIR}/dxl_x64_cpp.lib")

# Ensure the DLL is copied to the output directory after build
add_custom_command(TARGET MyProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MY_LIBRARY_DLL_DIR}/dxl_x64_cpp.dll"
        $<TARGET_FILE_DIR:MyProject>
)

# # Optionally, specify the output directory for the executable
# set_target_properties(MyProject PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
# )