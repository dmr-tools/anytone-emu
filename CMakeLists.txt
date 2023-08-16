cmake_minimum_required(VERSION 3.0.0)
project(anytone-emu VERSION 0.1.0)

set(RELEASE_SUFFIX "")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5SerialPort REQUIRED)

ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5SerialPort_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src)

set(CORE_LIBS ${Qt5Core_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES} ${Qt5SerialPort_LIBRARIES})
set(LIBS ${CORE_LIBS} ${Qt5Widgets_LIBRARIES})

set(PROJECT_VERSION_STRING "\"${PROJECT_VERSION}\"")
message(STATUS "Build version ${PROJECT_VERSION}")

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS} -Wall -Wsign-compare")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}  -O0 -ggdb -fstack-protector -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wextra")

# Sources...
add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(src)

# Source distribution packages:
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}${RELEASE_SUFFIX}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build/;/doc/html;/doc/latex;/doc/reveng;/doc/dmr-intro;/examples/*.dfu;/.git/;~$;.qm;*.user$;${CPACK_SOURCE_IGNORE_FILES}")

include(CPack)
