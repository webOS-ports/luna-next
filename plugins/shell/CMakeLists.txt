include_directories(
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    plugin.cpp
    reticleitem.cpp
    fpscounter.cpp
    gesturehandler.cpp
    devicekeyhandler.cpp
    volumekeys.cpp
    quickutils.cpp
    inversemouseareatype.cpp)

add_library(lunanext-shell-qml SHARED ${SOURCES})
target_link_libraries(lunanext-shell-qml ${LUNA_SYSMGR_COMMON_LIBRARIES}
    Qt5::Qml Qt5::Quick
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                                              "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Shell")
install(TARGETS lunanext-shell-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})

add_subdirectory(notifications)
