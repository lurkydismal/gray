project(xrCompress)
set(COMPR_SRC_FILES)

# Files import
file(GLOB_RECURSE COMPR_SOURCE_ALL_FILES CONFIGURE_DEPENDS
    "*.cpp"
    "*.c"
    "*.h"
)

# Source groups
source_group("kernel" FILES ${COMPR_SOURCE_ALL_FILES})

# Apply list
list(APPEND COMPR_SRC_FILES ${COMPR_SOURCE_ALL_FILES})

# xrCompress project
add_executable(xrCompress ${COMPR_SRC_FILES})
target_include_directories(xrCompress PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Move precompiled header
target_validate_pch(xrCompress ${CMAKE_CURRENT_SOURCE_DIR})

# Project defines
if (WIN32)
    target_compile_definitions(xrCompress PRIVATE _USRDLL)
endif()

# Linker list
target_link_libraries(xrCompress PUBLIC xrCore)

if (WIN32)
    target_link_libraries(xrCompress PRIVATE "winmm.lib")
endif()

# Set relative runpath
if(NOT WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH ""
        BUILD_RPATH_USE_ORIGIN ON
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "$ORIGIN"
    )
endif()
