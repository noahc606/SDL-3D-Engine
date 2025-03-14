cmake_minimum_required(VERSION 3.5)
set(PROJ_NAME "SDL-3D")

if (WIN32)
    set(PROJ_OUT "${PROJ_NAME}-windows-x86_64")
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif (WIN32)
if (UNIX)
    set(PROJ_OUT "${PROJ_NAME}-unix")
endif (UNIX)

# Project
project(${PROJ_OUT} VERSION 0.1.1.0 LANGUAGES C CXX)
set(CMAKE_C_FLAGS "-std=c++11 -o")

# Get list of sources and headers
file (GLOB_RECURSE PROJ_SOURCES FOLLOW_SYMLINKS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/include/*.cpp")
file (GLOB_RECURSE PROJ_HEADERS FOLLOW_SYMLINKS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h" "${CMAKE_SOURCE_DIR}/include/*.h")

# Build list of includes
set (PROJ_SRC_DIRS "")
foreach (_headerFile ${PROJ_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND PROJ_SRC_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES PROJ_SRC_DIRS)

# Add executable
add_executable(${PROJ_OUT} ${PROJ_SOURCES})
set_target_properties(${PROJ_OUT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Linking
target_link_libraries(${PROJ_OUT} PRIVATE "-lSDL2 -lSDL2_ttf -lSDL2_image -fopenmp")

# Add include directories (everywhere there is a header file: libraries and BTE src dirs)
target_include_directories(${PROJ_OUT} PRIVATE ${PROJ_SRC_DIRS})
target_include_directories(${PROJ_OUT} PRIVATE "include")