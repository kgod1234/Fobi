# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name
project(MyProject)

# Add the executable
add_executable(MyProject src/main.cpp src/Mylibs/SimplifyDynamixel.cpp)

# Specify paths to Dynamixel SDK
set(MY_LIBRARY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/DynamixelSDK-master/c++/include/dynamixel_sdk")
set(MY_LIBRARY_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/DynamixelSDK-master/c++/build/linux_sbc")
set(MY_CUSTOM_LIB_DIR "${CMAKE_SOURCE_DIR}/src/Mylibs") # Path to your custom Mylibs

# Include directories
target_include_directories(MyProject PRIVATE
    "${MY_LIBRARY_INCLUDE_DIR}"  # External Dynamixel SDK headers
    "${MY_CUSTOM_LIB_DIR}"       # Custom Mylibs headers
)

# Link the Dynamixel SDK shared library (.so file)
target_link_libraries(MyProject PRIVATE "${MY_LIBRARY_LIB_DIR}/libdxl_sbc_cpp.so")

# Optionally specify runtime library search path for the .so file
# Ensures the program finds the shared library at runtime
set_target_properties(MyProject PROPERTIES
    BUILD_RPATH "${MY_LIBRARY_LIB_DIR}"
)

# Optionally specify the output directory for the executable
set_target_properties(MyProject PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)