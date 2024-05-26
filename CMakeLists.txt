cmake_minimum_required(VERSION 3.11.3)

# Project definition
project(ContextManagementSystem
        VERSION 0.0.1
        LANGUAGES CXX)

find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Set ContextManagementSystem as the startup project (for Visual Studio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ContextManagementSystem)

# Globing source files
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Globing header files
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
                               ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

# Add executable
add_executable(ContextManagementSystem ${SOURCE_FILES} ${HEADER_FILES})

# Set C++ standard to C++23
target_compile_features(ContextManagementSystem PRIVATE cxx_std_23)

# Link spdlog
target_link_libraries(ContextManagementSystem PRIVATE spdlog::spdlog)

# Link nlohmann_json
target_link_libraries(ContextManagementSystem PRIVATE nlohmann_json::nlohmann_json)

# Create source groups for filters
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADER_FILES})

# Include directories
target_include_directories(ContextManagementSystem PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set the output directory for the executable
set_target_properties(ContextManagementSystem PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
