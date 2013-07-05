DEFINES += QT_COMPOSITOR_QUICK

QT += quick qml v8
QT += quick-private

QT += compositor

SOURCES += src/main.cpp
OTHER_FILES = \
    src/ContrastEffect.qml  \
    src/main.qml \
    src/WindowChrome.qml \
    src/WindowContainer.qml \
    src/background.jpg \
    src/closebutton.png
    src/compositor.js
