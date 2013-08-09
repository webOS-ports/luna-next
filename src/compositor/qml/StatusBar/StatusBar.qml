import QtQuick 2.0

Item {
    id: statusBarDisplay

    Rectangle {
        anchors.fill: statusBarDisplay
        color: "black"

        Text {
            anchors.centerIn: parent
            color: "white"
            font.family: "Prelude"
            font.pixelSize: 24
            font.bold: true
            text: Qt.formatDateTime(new Date(), "dd.MM.yyyy")
        }
    }
}
