cmake_minimum_required(VERSION 3.13)

# Project name
project(meshtastic_solar)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SOURCES
    src/main.cpp
    src/meshtastic_handler.cpp
)

# Headers
set(HEADERS
    include/config.h
    include/meshtastic_handler.h
)

# Note: This CMakeLists.txt is provided for reference and potential
# integration with the Meshtastic firmware build system, which uses CMake.
# 
# For building this standalone project, use PlatformIO instead:
#   pio run -e rak4631
#
# If integrating with Meshtastic firmware:
# 1. Copy source files to firmware/src/modules/EmergencyModule/
# 2. Update this CMakeLists.txt to match Meshtastic's structure
# 3. Add to firmware's main CMakeLists.txt:
#    add_subdirectory(src/modules/EmergencyModule)

# Placeholder executable (won't build without proper Arduino/nRF52 toolchain)
# add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# For Meshtastic firmware integration, you would typically:
# - Create a library target
# - Link against Meshtastic core libraries
# - Add platform-specific flags

# Example for Meshtastic integration:
# add_library(emergency_module STATIC ${SOURCES})
# target_include_directories(emergency_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(emergency_module mesh_core u8g2)
