add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/compositor)

set(SOURCES
    main.cpp
    compositor/compositor.cpp
    compositor/compositorwindow.cpp)

qt5_add_resources(RESOURCES compositor/compositor.qrc)

add_executable(luna-next ${SOURCES} ${RESOURCES})
qt5_use_modules(luna-next Quick Compositor Gui)
target_link_libraries(luna-next
    ${SYSTEMD_DAEMON_LIBRARIES}
    ${LUNA_SYSMGR_COMMON_LIBRARIES})

webos_build_daemon()
webos_build_system_bus_files()
