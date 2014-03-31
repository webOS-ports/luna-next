add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/compositor)

set(SOURCES
    plugin.cpp
    compositor.cpp
    compositorwindow.cpp
    windowmodel.cpp
    windowstate.cpp
    windowtype.cpp
    screenshooter.cpp
    eventtype.cpp
    displaycontroller.cpp)

add_library(lunanext-compositor-qml SHARED ${SOURCES})
qt5_use_modules(lunanext-compositor-qml Qml Quick Compositor)

execute_process(COMMAND ${CMAKE_COMMAND}
                -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                     "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Compositor")
install(TARGETS lunanext-compositor-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
