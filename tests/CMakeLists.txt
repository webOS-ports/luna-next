include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS})

set(TEST_WEBAPP_SOURCES test_webapp.cpp)

qt5_add_resources(TEST_WEBAPP_RESOURCES test_webapp.qrc)

add_executable(test_webapp ${TEST_WEBAPP_SOURCES} ${TEST_WEBAPP_RESOURCES})
qt5_use_modules(test_webapp Quick Gui WebKit)

install(TARGETS test_webapp DESTINATION bin)
