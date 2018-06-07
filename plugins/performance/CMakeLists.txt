include_directories(
    ${GLIB2_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    rendertimer.cpp
    cpuusage.cpp
    graphmodel.cpp
    plugin.cpp
    renderingtimes.cpp
    texturefromimage.cpp)

set(QML_SOURCES
    BarGraph.qml
    PerformanceOverlay.qml)

add_library(lunanext-performance-qml SHARED ${SOURCES})
target_link_libraries(lunanext-performance-qml ${LUNA_SYSMGR_COMMON_LIBRARIES} ${EGL_LIBRARIES}
    Qt5::Qml Qt5::Quick
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
                                                              "${CMAKE_CURRENT_BINARY_DIR}/qmldir")

# FIXME Using ${OE_QMAKE_PATH_QML} isn't really portable but the only way atm
set(QMLPLUGIN_INSTALL_PREFIX "${OE_QMAKE_PATH_QML}/LunaNext/Performance")
install(TARGETS lunanext-performance-qml DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES ${QML_SOURCES} DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
install(FILES qmldir DESTINATION ${QMLPLUGIN_INSTALL_PREFIX})
