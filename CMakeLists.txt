project(luna-next)

cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(LUNA_NEXT_VERSION_MAJOR 0)
set(LUNA_NEXT_VERSION_MINOR 1)
set(LUNA_NEXT_VERSION_PATCH 0)
set(LUNA_NEXT_VERSION_STRING "${LUNA_NEXT_VERSION_MAJOR}.${LUNA_NEXT_VERSION_MINOR}.${LUNA_NEXT_VERSION_PATCH}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -std=c++0x")

find_package(Qt5Gui REQUIRED)
if(NOT Qt5Gui_FOUND)
    message(FATAL_ERROR "Qt5Gui module is required!")
endif()

find_package(Qt5Qml REQUIRED)
if(NOT Qt5Qml_FOUND)
    message(FATAL_ERROR "Qt5Qml module is required!")
endif()

find_package(Qt5Quick REQUIRED)
if(NOT Qt5Quick_FOUND)
    message(FATAL_ERROR "Qt5Quick module is required!")
endif()

find_package(Qt5Compositor REQUIRED)
if(NOT Qt5Compositor_FOUND)
    message(FATAL_ERROR "Qt5Compositor module is required!")
endif()

find_package(PkgConfig "0.22" REQUIRED)

pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLESV2 glesv2 REQUIRED)

pkg_check_modules(WAYLAND_CLIENT wayland-client REQUIRED)
pkg_check_modules(WAYLAND_SERVER wayland-server REQUIRED)

add_subdirectory(src)
