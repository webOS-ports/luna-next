import QtQuick 2.0

Item {
    id: gestureAreaDisplay
    property alias mouseArea: mouseArea

    signal clickGestureArea

    // Black rectangle behind, and a glowing image in front
    Rectangle {
        anchors.fill: parent
        color: "black"
    }
    Image {
        anchors.fill: parent
        source: "glow.png"
        smooth: true
        fillMode: Image.Stretch
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: clickGestureArea()
    }
}
