import QtQuick 2.0

Item {
    id: gestureAreaDisplay

    Rectangle {
        anchors.fill: parent
    }
    MouseArea {
        anchors.fill: parent

        onClicked: {
            // leave fullscreen
            cardViewDisplay.setCurrentWindowMaximizedState(false);
        }
    }
}
