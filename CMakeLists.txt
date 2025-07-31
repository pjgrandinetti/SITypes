cmake_minimum_required(VERSION 3.15)
project(SITypes C)
set(CMAKE_C_STANDARD 11)

# -------------------------------------------------------------------
# 1) Find Flex and Bison
# -------------------------------------------------------------------
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# -------------------------------------------------------------------
# 2) Tell CMake to generate parser & scanners into the current binary dir
#    (When you later run `cmake -G "Xcode" -B build-xcode`, 
#    CMAKE_CURRENT_BINARY_DIR == SITypes/build-xcode.)
# -------------------------------------------------------------------

BISON_TARGET(
    SIDIM_PARSER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionalityParser.y
    ${CMAKE_CURRENT_BINARY_DIR}/SIDimensionalityParser.tab.c
    DEFINES_FILE
      ${CMAKE_CURRENT_BINARY_DIR}/SIDimensionalityParser.tab.h
)

FLEX_TARGET(
    SIDIM_SCANNER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionalityScanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/SIDimensionalityScanner.c
)

FLEX_TARGET(
    SIFORM_SCANNER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIFormulaScanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/SIFormulaScanner.c
)

# new SIUnit parser/scanner
BISON_TARGET(
    SIUNIT_PARSER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnitParser.y
    ${CMAKE_CURRENT_BINARY_DIR}/SIUnitParser.tab.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/SIUnitParser.tab.h
)

FLEX_TARGET(
    SIUNIT_SCANNER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnitScanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/SIUnitScanner.c
)
ADD_FLEX_BISON_DEPENDENCY(SIUNIT_SCANNER SIUNIT_PARSER)

# new SIScalar parser/scanner
BISON_TARGET(
    SISCALAR_PARSER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarParser.y
    ${CMAKE_CURRENT_BINARY_DIR}/SIScalarParser.tab.c
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/SIScalarParser.tab.h
)

FLEX_TARGET(
    SISCALAR_SCANNER
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarScanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/SIScalarScanner.c
)
ADD_FLEX_BISON_DEPENDENCY(SISCALAR_SCANNER SISCALAR_PARSER)

# -------------------------------------------------------------------
# 3) List your “hand‐written” .c/.h files
# -------------------------------------------------------------------
set(SITYPE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SILibrary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionality.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionalityLib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIQuantity.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarParser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarConstants.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarParserHelpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnitExpression.c
)

set(SITYPE_HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionality.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionalityPrivate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIDimensionalityParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIQuantity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SILibrary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarConstants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIScalarConstantsData.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnitParser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SIUnitExpression.h
)

# -------------------------------------------------------------------
# 4) Combine everything into a single STATIC library
# -------------------------------------------------------------------
set(ALL_SOURCES
    ${SITYPE_SRC}
    ${SITYPE_HDR}
    ${BISON_SIDIM_PARSER_OUTPUTS}
    ${FLEX_SIDIM_SCANNER_OUTPUTS}
    ${FLEX_SIFORM_SCANNER_OUTPUTS}
    ${BISON_SIUNIT_PARSER_OUTPUTS}
    ${FLEX_SIUNIT_SCANNER_OUTPUTS}
    ${BISON_SISCALAR_PARSER_OUTPUTS}
    ${FLEX_SISCALAR_SCANNER_OUTPUTS}
)

add_library(SITypes STATIC ${ALL_SOURCES})

# Expose public headers in Xcode and make install target
set_target_properties(SITypes PROPERTIES
    PUBLIC_HEADER "${SITYPE_HDR}"
)

# Install static library and headers
install(TARGETS SITypes
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/SITypes
)

# -------------------------------------------------------------------
# 5) Include directories:
#    - <source>/src for your own headers,
#    - <binary> (i.e. build-xcode) so C code can #include "SIDimensionalityParser.tab.h"
# -------------------------------------------------------------------
target_include_directories(SITypes PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# -------------------------------------------------------------------
# 6) If someone has already built OCTypes in the same CMake run,
#    link against it.  (This is optional but convenient if you do a super‐project.)
# -------------------------------------------------------------------
if (TARGET OCTypes)
    target_link_libraries(SITypes PUBLIC OCTypes)
endif()

# Fall back to third_party/OCTypes if no OCTypes target
if (NOT TARGET OCTypes)
    message(STATUS "Linking against OCTypes in third_party/OCTypes")
    target_include_directories(SITypes PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/OCTypes/include
    )
    target_link_libraries(SITypes PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/OCTypes/lib/libOCTypes.a
    )
endif()

# -------------------------------------------------------------------
# 8) Tests executable for Xcode and CTest
# -------------------------------------------------------------------
enable_testing()
file(GLOB SITYPE_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
)
add_executable(SITypes_runTests ${SITYPE_TEST_SOURCES})
target_include_directories(SITypes_runTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/OCTypes/src
    ${CMAKE_BINARY_DIR}/OCTypes
)
target_link_libraries(SITypes_runTests PRIVATE
    OCTypes
    SITypes
    m
)
add_test(NAME SITypes_runTests COMMAND SITypes_runTests)

# -------------------------------------------------------------------
# 9) (Optional) Organize Xcode groups so headers & sources appear under their own folders
# -------------------------------------------------------------------
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source Files"
    FILES ${SITYPE_SRC}
)

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Header Files"
    FILES ${SITYPE_HDR}
)

# Put generated files under a “Generated” group
source_group(
    TREE ${CMAKE_CURRENT_BINARY_DIR}
    PREFIX "Generated"
    FILES
      ${BISON_SIDIM_PARSER_OUTPUTS}
      ${FLEX_SIDIM_SCANNER_OUTPUTS}
      ${FLEX_SIFORM_SCANNER_OUTPUTS}
      ${BISON_SIUNIT_PARSER_OUTPUTS}
      ${FLEX_SIUNIT_SCANNER_OUTPUTS}
      ${BISON_SISCALAR_PARSER_OUTPUTS}
      ${FLEX_SISCALAR_SCANNER_OUTPUTS}
)
