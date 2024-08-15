cmake_minimum_required (VERSION 2.6)

project (vermilion9)

LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

set (CMAKE_DEBUG_POSTFIX "_d")

find_package(OpenGL REQUIRED)

if(WIN32)
set(COMMON_LIBS vermilion ${OPENGL_LIBRARIES} optimized glfw3 debug glfw3_d)
elseif (UNIX)
set(COMMON_LIBS vermilion ${OPENGL_LIBRARIES} glfw ${GLFW_LIBRARIES} GL rt dl)
else()
set(COMMON_LIBS vermilion)
endif()

set(COMMON_HEADERS include/vgl.h include/vapp.h)

set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

add_library(vermilion
            lib/gl3w.c
            lib/LoadShaders.cpp
            lib/targa.cpp
            lib/vdds.cpp
            lib/loadtexture.cpp
            lib/vermilion.cpp
            lib/vbm.cpp
)

set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Weight blended OIT example.
set (OIT_EXAMPLE_NAME exp-weight-blended-oit)
set (OIT_EXAMPLE_HEADERS
  ${COMMON_HEADERS}
  src/${OIT_EXAMPLE_NAME}/SceneCreator.h
  src/${OIT_EXAMPLE_NAME}/WeightBlendedOitExample.h
  src/${OIT_EXAMPLE_NAME}/VBObjectExt.h
)
set (OIT_EXAMPLE_SRC
src/${OIT_EXAMPLE_NAME}/Shaders.cpp
src/${OIT_EXAMPLE_NAME}/SceneCreator.cpp
src/${OIT_EXAMPLE_NAME}/VBObjectExt.cpp
src/${OIT_EXAMPLE_NAME}/WeightBlendedOitExample.cpp
)
add_executable(${OIT_EXAMPLE_NAME} WIN32 ${OIT_EXAMPLE_SRC} ${OIT_EXAMPLE_HEADERS})
set_property(TARGET ${OIT_EXAMPLE_NAME} PROPERTY DEBUG_POSTFIX _d)
target_link_libraries(${OIT_EXAMPLE_NAME} ${COMMON_LIBS})

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

include_directories( include )
include_directories(lib/glfw/include)

add_subdirectory(lib/glfw)

ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR})
ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR})
