import QtQuick 2.0

Item {
    id: dummyWindow

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "black" }
        }
    }
}
