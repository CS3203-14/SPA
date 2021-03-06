cmake_minimum_required (VERSION 3.9.2)
project (autotester)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
message (STATUS "${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}")
if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are forbidden.")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_BUILD_TYPE "") # Messes up autotester if the build type is really set
  message("\n\n*********************\n\nDEBUG Build Type\n\n*********************\n\n")
else()
  message("\n\n*********************\n\nRELEASE Build Type\n\n*********************\n\n")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -Wcast-qual -Wcast-align -Wstrict-aliasing -Wpointer-arith -Winit-self -Wshadow -Wswitch-enum -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wfloat-equal -Wundef -Wvla -Wdeclaration-after-statement -Wc++-compat")

if (PROFILING)
  message("******* Profiling active *********")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -pg")
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g -pg")
endif()

if (WIN32)
  SET(CMAKE_FIND_LIBRARY_PREFIXES "")
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
  set(AUTOTESTER_ROOT "${CMAKE_CURRENT_LIST_DIR}/lib/autotester/win")
elseif (APPLE)
  set(Qt5Core_DIR /usr/local/opt/qt/lib/cmake/Qt5Core)
  set(Qt5Widgets_DIR /usr/local/opt/qt/lib/cmake/Qt5Widgets)
  set(Qt5Gui_DIR /usr/local/opt/qt/lib/cmake/Qt5Gui)
  set(AUTOTESTER_ROOT "${CMAKE_CURRENT_LIST_DIR}/lib/autotester/apple")
elseif (UNIX)
  set(AUTOTESTER_ROOT "${CMAKE_CURRENT_LIST_DIR}/lib/autotester/unix")
endif()


list(APPEND CMAKE_PREFIX_PATH "${AUTOTESTER_ROOT}")
find_package(Autotester REQUIRED)
include_directories("${CMAKE_CURRENT_LIST_DIR}/lib")#include catch.hpp

add_subdirectory(src/spa)
add_subdirectory(src/autotester)
if (BUILD_GUI)
  add_subdirectory(src/autotester_gui)
endif()
add_subdirectory(src/unit_testing)
add_subdirectory(src/integration_testing)
add_subdirectory(src/viz)
if (PROFILING)
  add_subdirectory(src/profiling)
endif()

enable_testing()
add_test(NAME UnitTests COMMAND unit_testing WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME IntegrationTests COMMAND integration_testing WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
