include_directories(
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    recorder.cpp)

ecm_add_wayland_client_protocol(SOURCES
    PROTOCOL ${CMAKE_SOURCE_DIR}/protocol/recorder.xml
    BASENAME luna-recorder)

add_executable(luna-recorder ${SOURCES})
target_link_libraries(luna-recorder
    ${WAYLAND_CLIENT_LIBRARIES}
    Qt5::Gui
)
install(TARGETS luna-recorder DESTINATION bin)
