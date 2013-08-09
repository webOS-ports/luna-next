import QtQuick 2.0

Item {
    id: root
    property int frameCounter: 0
    property int fps: 0;

    Image {
        id: spinnerImage
        source: "../images/spinner.png"
        NumberAnimation on rotation {
            from:0
            to: 360
            duration: 800
            loops: Animation.Infinite
        }
        onRotationChanged: frameCounter++
    }

    Text {
        anchors.left: spinnerImage.right
        anchors.verticalCenter: spinnerImage.verticalCenter
        color: "red"
        font.pixelSize: 20
        text: root.fps + " fps"
    }

    Timer {
        interval: 2000
        repeat: true
        running: true
        onTriggered: {
            fps = frameCounter/2;
            frameCounter = 0;
        }
    }
}
