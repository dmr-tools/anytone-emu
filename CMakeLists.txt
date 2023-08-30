cmake_minimum_required(VERSION 3.0.0)
project(anytone-emu VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

include(GNUInstallDirs)
include(GenerateIcons)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort Test)

find_package(DocBookXSL REQUIRED)
include(DocBook)

cmake_policy(SET CMP0100 NEW)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(USE_FOLDERS ON)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/src)

message(STATUS "Build version ${PROJECT_VERSION}")

# Sources...
add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(doc)

# Source distribution packages:
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${PROJECT_VERSION}${RELEASE_SUFFIX}")
set(CPACK_SOURCE_IGNORE_FILES
  "/build/;/doc/html;/doc/latex;/doc/reveng;/doc/dmr-intro;/examples/*.dfu;/.git/;~$;.qm;*.user$;${CPACK_SOURCE_IGNORE_FILES}")

include(CPack)
