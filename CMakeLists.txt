project(luna-next)

cmake_minimum_required(VERSION 2.8.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC3)
webos_component(0 2 0)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

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

find_package(Qt5WaylandCompositor REQUIRED)
if(NOT Qt5WaylandCompositor_FOUND)
    message(FATAL_ERROR "Qt5WaylandCompositor module is required!")
endif()

find_package(Qt5DBus REQUIRED)
if(NOT Qt5DBus_FOUND)
    message(FATAL_ERROR "Qt5DBus module is required!")
endif()

find_package(Qt5Sql REQUIRED)
if(NOT Qt5Sql_FOUND)
    message(FATAL_ERROR "Qt5Sql module is required!")
endif()

find_package(PkgConfig "0.22" REQUIRED)

pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(GLESV2 glesv2 REQUIRED)

pkg_check_modules(WAYLAND_CLIENT wayland-client REQUIRED)
pkg_check_modules(WAYLAND_SERVER wayland-server REQUIRED)

pkg_check_modules(SYSTEMD libsystemd REQUIRED)
pkg_check_modules(GLIB2 glib-2.0 REQUIRED)
pkg_check_modules(LUNA_SYSMGR_COMMON LunaSysMgrCommon REQUIRED)
pkg_check_modules(LUNA_SYSMGR_IPC_MESSAGES LunaSysMgrIpcMessages REQUIRED)
pkg_check_modules(LUNA_PREFS luna-prefs REQUIRED)

include(FindWaylandScanner)
include(FindQtWaylandScanner)

add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(tools)

webos_build_system_bus_files()
