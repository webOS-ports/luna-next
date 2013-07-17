import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    property variant content
    property alias radius: roundedCornerMask.radius

    Rectangle {
        id: roundedCornerMask
        anchors.fill: parent
        color: "black"
        visible: false
    }
    OpacityMask {
        anchors.fill: parent
        source: content
        maskSource: roundedCornerMask
    }

    Component.onCompleted: content.visible = false
}
