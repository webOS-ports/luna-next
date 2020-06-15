include_directories(
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${LUNA_SYSMGR_IPC_MESSAGES_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    plugin.cpp
    settingsadapter.cpp
    ledsadapter.cpp
    fontutils.cpp
    fileutils.cpp
    units.cpp)

add_library(lunanext-common-qml SHARED ${SOURCES})
target_link_libraries(lunanext-common-qml
    ${LUNA_SYSMGR_COMMON_LIBRARIES} ${LS2_LIBRARIES} ${LUNA_PREFS_LIBRARIES}
    Qt5::Qml Qt5::Quick
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                                              "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Common")
install(TARGETS lunanext-common-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
