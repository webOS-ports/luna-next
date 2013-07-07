add_definitions(-DQT_COMPOSITOR_QUICK)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${Qt5Compositor_PRIVATE_INCLUDE_DIRS}
    ${WAYLAND_SERVER_INCLUDE_DIR})

set(SOURCES
    main.cpp)

add_executable(luna-next ${SOURCES})
qt5_use_modules(luna-next Quick Compositor Gui)

# install(TARGETS luna-next DESTINATION ${CMAKE_INSTALL_BINDIR})
