include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    shellapplication.cpp)

add_executable(luna-next ${SOURCES} ${RESOURCES})
target_link_libraries(luna-next
    ${SYSTEMD_LIBRARIES}
    ${LUNA_SYSMGR_COMMON_LIBRARIES}
    ${GLIB2_LIBRARIES}
    Qt5::Quick
    Qt5::Gui
)

webos_build_daemon()
webos_build_system_bus_files()
