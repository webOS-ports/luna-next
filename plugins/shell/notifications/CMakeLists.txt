include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    plugin.cpp
    qobjectlistmodel.cpp
    categorydefinitionstore.cpp
    notification.cpp
    notificationmanager.cpp
    notificationmanagerwrapper.cpp
    notificationlistmodel.cpp)

#qt5_add_dbus_adaptor(SOURCES notificationmanager.xml notificationmanager.h NotificationManager
#                     notificationmanageradaptor NotificationManagerAdaptor)

add_library(lunanext-shell-notifications-qml SHARED ${SOURCES})
set_target_properties(lunanext-shell-notifications-qml
    PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DQT_DISABLE_DEPRECATED_BEFORE=0x000000")
target_link_libraries(lunanext-shell-notifications-qml ${LUNA_SYSMGR_COMMON_LIBRARIES}
    Qt5::Qml Qt5::Quick Qt5::Sql
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                                              "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Shell/Notifications")
install(TARGETS lunanext-shell-notifications-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
