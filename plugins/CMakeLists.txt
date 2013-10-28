add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${LUNA_SYSMGR_IPC_MESSAGES_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/compositor)

set(SOURCES
    lunanextplugin.cpp
    compositor/compositor.cpp
    compositor/compositorwindow.cpp
    settingsadapter.cpp
    windowstate.cpp
    windowtype.cpp
    reticleitem.cpp
    lunaserviceadapter.cpp
    settingsadapter.cpp
    fpscounter.cpp
    screenshooter.cpp
    eventtype.cpp
    btdeviceclass.cpp
    deviceinfo.cpp
    statusbarservicesconnector.cpp)

add_library(lunanext-qml SHARED ${SOURCES})
target_link_libraries(lunanext-qml ${LUNA_SYSMGR_COMMON_LIBRARIES} ${LS2_LIBRARIES} ${LUNA_PREFS_LIBRARIES})

qt5_use_modules(lunanext-qml Qml Quick Compositor)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                                              "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext")
install(TARGETS lunanext-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
