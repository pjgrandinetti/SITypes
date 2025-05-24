cmake_minimum_required(VERSION 3.15)
project(SITypes C)
set(CMAKE_C_STANDARD 11)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Generate parser and scanner sources in build dir
BISON_TARGET(SIDIM_PARSER src/SIDimensionalityParser.y
    ${CMAKE_CURRENT_BINARY_DIR}/SIDimensionalityParser.tab.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/SIDimensionalityParser.tab.h
)
FLEX_TARGET(SI_SCANNER src/SIScanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/SIScanner.c
)
ADD_FLEX_BISON_DEPENDENCY(SI_SCANNER SIDIM_PARSER)

# Collect static source files (exclude generated)
file(GLOB STATIC_SOURCES src/*.c)
list(REMOVE_ITEM STATIC_SOURCES
    src/SIScanner.c
    src/SIDimensionalityParser.tab.c
)

# Combine all sources
set(ALL_SOURCES
    ${BISON_SIDIM_PARSER_OUTPUTS}
    ${FLEX_SI_SCANNER_OUTPUTS}
    ${STATIC_SOURCES}
)

# Build static library
add_library(SITypes STATIC ${ALL_SOURCES})

target_include_directories(SITypes PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)
