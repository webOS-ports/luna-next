import QtQuick 2.0

Item {
    property real cornerRadius

    Image {
        anchors.top: parent.top
        anchors.left: parent.left
        width: cornerRadius
        height: cornerRadius
        source: "../images/wm-corner-top-left.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.top: parent.top
        anchors.right: parent.right
        width: cornerRadius
        height: cornerRadius
        source: "../images/wm-corner-top-right.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: cornerRadius
        height: cornerRadius
        source: "../images/wm-corner-bottom-left.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: cornerRadius
        height: cornerRadius
        source: "../images/wm-corner-bottom-right.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
}
