add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src/compositor)

set(SOURCES
    main.cpp
    compositor/compositor.cpp
    compositor/compositorwindow.cpp)

qt5_add_resources(RESOURCES compositor/compositor.qrc)

add_executable(luna-next ${SOURCES} ${RESOURCES})
qt5_use_modules(luna-next Quick Compositor Gui)

install(TARGETS luna-next DESTINATION bin)
