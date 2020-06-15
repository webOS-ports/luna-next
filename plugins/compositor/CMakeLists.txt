add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5WaylandCompositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/compositor)

set(SOURCES
    plugin.cpp
    compositor.cpp
    compositorwindow.cpp
    recorder.cpp
    windowmodel.cpp
    windowstate.cpp
    windowtype.cpp
    screenshooter.cpp
    eventtype.cpp
    displaycontroller.cpp)

ecm_add_qtwayland_server_protocol(SOURCES
    PROTOCOL ${CMAKE_SOURCE_DIR}/protocol/recorder.xml
    BASENAME luna-recorder
    PREFIX luna_)

add_library(lunanext-compositor-qml SHARED ${SOURCES})
target_link_libraries(lunanext-compositor-qml
    Qt5::Qml Qt5::Quick Qt5::WaylandCompositor
)

execute_process(COMMAND ${CMAKE_COMMAND}
                -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                     "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Compositor")
install(TARGETS lunanext-compositor-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
