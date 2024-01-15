cmake_minimum_required(VERSION 3.10)
project(ElementaryCellularAutomata C)

set(CMAKE_C_STANDARD 11)
set(OUTPUT_NAME simulate)

# Explicitly list your source files
set(SOURCES
    src/main.c
)

# Find SDL2
find_library(SDL2_LIBRARY SDL2)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    include
)

# Executable
add_executable(${OUTPUT_NAME} ${SOURCES})

# Link SDL2
target_link_libraries(${OUTPUT_NAME} ${SDL2_LIBRARY})
